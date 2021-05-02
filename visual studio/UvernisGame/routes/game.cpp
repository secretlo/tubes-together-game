namespace RouteGame {
   std::string pwd = File::pwd();
   
   void response(Response& res, std::string action, json data) {
      data["action"] = action;
      res.set_content(data.dump().c_str(), "text/json; charset=UTF-8");
   }
   
   int randint(int min, int max) {
      return min + (std::rand() % static_cast<int>(max - min + 1));
   }
   
   void sendAll(json& game, std::string action, json data) {
      data["action"] = action;
      std::cout << "Game, sendAll: action '" << action << "' with data " << data.dump() << '\n';

      for (json user : game["users"]) {
         std::string userHost = user["host"].get<std::string>();
         userHost = "http://" + userHost;
         Client* userServer = new Client(userHost.c_str());
         userServer->Post("/", data.dump(), "application/json");
      }
   }
   

   size_t getReadyCount(json& game) {
      size_t readyCount = 0;
      ::jsonForEach(&game["users"], [&](json& user){
         if (user["ready"].get<bool>())
            readyCount++;
      });
      return readyCount;
   }
   size_t getConnectionCount(json& game) {
      return game["users"].size();
   }
   
   
   void PostHandler(const Request& req, Response& res) {
      std::cout << "Game: Post\n";

      size_t sessionID = std::stoi(getParam(req, "session_id"));
      std::string host = getParam(req, "host");
      std::string action = getParam(req, "action");
      File* games = new File("..\\data\\games.json");
      bool isDeleteGame = false;
      size_t deleteGameId;

      std::cout << "Game: Action '" << action << "' from user (host: " << host << ", sessionID: " << sessionID << ")\n";
      
      games->updateJson([&](json* games){
         ::jsonForSome(games, "id", sessionID, [&](json& game){
            
            // Action: Position-update
            if (action == "position-update") {
               std::string x = getParam(req, "x"), y = getParam(req, "y");

               response(res, "position-update", json{});
               
               sendAll(game, "position-update", json{
                  {"host", host},
                  {"x", x},
                  {"y", y},
               });
            }
            
            // Action: Create-enemy
            else if (action == "create-enemy") {
               int x = std::stoi(getParam(req, "x")), y = std::stoi(getParam(req, "y"));
               int resX, resY, randPercent = randint(0, 100);
               int side = randint(1, 4);

               if (side == 1) { // bottom
                  resY = 0;
                  resX = randPercent;
               } else if (side == 3) { // top
                  resY = 100;
                  resX = randPercent;
               } else if (side == 2) { // left
                  resX = 0;
                  resY = randPercent;
               } else if (side == 4) { // right
                  resX = 100;
                  resY = randPercent;
               }
               
               int vx = x - resX, vy = y - resY;
               
               int resSpeed = randint(1, 3);
               
               response(res, action, json{});
               sendAll(game, action, json{
                  {"x", resX},
                  {"y", resY},
                  {"vx", vx},
                  {"vy", vy},
                  {"speed", resSpeed},
               });
            }
            
            // Action: Die
            else if (action == "die") {
               response(res, action, json{});
               sendAll(game, action, json{
                  {"host", host},
               });
            }

            // Action: Connect
            else if (action == "connect") {
               std::cout << "Game: Connect action, with host " << host << "\n";

               json defaultUser = File::ReadJson("..\\data\\default-game-user.json");
               defaultUser["host"] = host;

               game["users"].push_back(defaultUser);
               size_t connectionCount = getConnectionCount(game);
               
               std::cout << "Game: Connection count become " << connectionCount << "\n";

               response(res, action, json{
                  {"status", "OK"},
                  {"connection_count", connectionCount},
               });
               
               sendAll(game, "connection-count-update", json{
                  {"count", connectionCount},
               });
            }

            // Action: Get-ready-count
            else if (action == "get-ready-count") {
               response(res, action, json{
                  {"count", getReadyCount(game)},
               });
            }
            
            // Action: Ready
            else if (action == "ready") {
               response(res, action, json{});
               
               ::jsonForSome(&game["users"], "host", host, [&](json& user){
                  user["ready"] = true;
               });
               
               size_t readyCount = getReadyCount(game);
               size_t connectionCount = getConnectionCount(game);
               
               sendAll(game, "ready-count-update", json{
                  {"count", readyCount},
               });

               if (readyCount == connectionCount) {
                  sendAll(game, "game-start", json{
                     {"players", game["users"]},
                  });
               }
            }

            // Action: Unready
            else if (action == "unready") {
               response(res, action, json{});

               ::jsonForSome(&game["users"], "host", host, [&](json& user){
                  user["ready"] = false;
               });

               size_t readyCount = getReadyCount(game);
               sendAll(game, "ready-count-update", json{
                  {"count", readyCount},
               });
            }
            
            // Action: Disconnect
            else if (action == "disconnect") {
               response(res, action, json{});
               
               ::jsonDeleteSome(&game["users"], "host", host);

               size_t readyCount = getReadyCount(game);
               size_t connectionCount = getConnectionCount(game);

               if (connectionCount > 0) {
                  sendAll(game, "ready-count-update", json{
                     {"count", readyCount},
                  });
                  sendAll(game, "connection-count-update", json{
                     {"count", connectionCount},
                  });
               }

               if (connectionCount == 0) {
                  isDeleteGame = true;
                  deleteGameId = game["id"].get<size_t>();
               }
            }
            
         }); // jsonForSome (game with id) end

         if (isDeleteGame)
            ::jsonDeleteSome(games, "id", deleteGameId);
      }); // games->updateJson (games json) end

   }
   
   
} // RouteGame