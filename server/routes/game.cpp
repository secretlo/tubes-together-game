namespace RouteGame {
   std::string pwd = File::pwd();
   
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
   
   json generateField() {
      json cells = json::array({ "lr", "tb", "lrt", "lrb", "ltb", "rtb", "lrtb" });
      json field = json::array();

      for (int y = 0; y < 7; y++) {
         field.push_back(json::array());
         for (int x = 0; x < 11; x++) {
            field[y][x] = cells[randint(0, 6)].get<std::string>();
         }
      }
      
      return field;
   }
   

   void PostHandler(const Request& req, Response& res) {
      std::cout << "Game: Post\n";

      size_t id = std::stoi(getParam(req, "id"));
      std::string action = getParam(req, "action");
      
      File* games = new File(pwd + "/../data/games.json"),
         * users = new File(pwd + "/../data/users.json");

      std::cout << "Game: Action '" << action << "' from user (id: " << id << ")\n";
      
      if (action == "wait") {
         std::string host = getParam(req, "host");
         size_t level = std::stoi(getParam(req, "level"));

         users->updateJson([&](json* users){
            json* user;
            jsonForSome(users, "id", id, [&](json& _user){ user = &_user; });
            user->at("host") = host;

            json* partner = nullptr;
            jsonForSome(users, 
               [&](json& user){
                  return user["isWait"].get<bool>() && user["levels"].get<size_t>() == level;
               }, 
               [&](json& user){
                  partner = &user;
               }
            );
            
            if (partner != nullptr) {
               partner->at("isWait") = false;
               user->at("isWait") = false;

               json defaultGame = File::ReadJson(pwd + "/../data/default-game.json"),
                  defaultGameUser;
               
               size_t sessionID;
               
               games->updateJson([&](json* games){
                  defaultGameUser = File::ReadJson(pwd + "/../data/default-game-user.json");
                  defaultGameUser["id"] = user->at("id").get<size_t>();
                  defaultGameUser["host"] = user->at("host").get<std::string>();
                  defaultGame["users"].push_back(defaultGameUser);

                  defaultGameUser = File::ReadJson(pwd + "/../data/default-game-user.json");
                  defaultGameUser["id"] = partner->at("id").get<size_t>();
                  defaultGameUser["host"] = partner->at("host").get<std::string>();
                  defaultGame["users"].push_back(defaultGameUser);
                  
                  size_t maxID = 0;
                  jsonForEach(games, [&](json& game){
                     size_t sessionID = game["id"].get<size_t>();
                     if (maxID < sessionID) maxID = sessionID;
                  });
                  sessionID = maxID + 1;
                  defaultGame["id"] = sessionID;
                  
                  games->push_back(defaultGame);
                  jsonForSome(games, "id", sessionID, [&](json& game){
                     json field = generateField();
                     
                     response(res, "Connected", action, json{
                        {"session_id", sessionID},
                        {"players", game["users"]},
                        {"field", field},
                     });
                     sendAll(game, "start", json{
                        {"session_id", sessionID},
                        {"players", game["users"]},
                        {"field", field},
                     });
                  });
               });
            } // if partner
            else { // else if NO partner
               user->at("isWait") = true;
               user->at("host") = host;
               response(res, "Wait", action);
               //response(res, "Connected", action);
            }
         }); // users->updateJson end
      }
      else if (action == "unwait") {
         users->updateJson([&](json* users){
            jsonForSome(users, "id", id, [&](json& user){
               user["isWait"] = false;
            });
         });
      }

      // Game events
      else {
         size_t sessionID = std::stoi(getParam(req, "session_id"));

         std::cout << "Game: Action '" << action << "' from user (id: " << id << ", sessionID: " << sessionID << ")\n";
         File* games = new File(pwd + "/../data/games.json");

         games->updateJson([&](json* games){
            jsonForSome(games, "id", sessionID, [&](json& game){
               
               // Action: Rotate
               if (
                  action == "rotate" ||
                  action == "hover" ||
                  action == "unhover"
               ) {
                  size_t x = std::stoi(getParam(req, "x")), 
                         y = std::stoi(getParam(req, "y"));
                  std::string senderID = getParam(req, "sender");
                         
                  std::cout << "Game, " << action << ": x is " << x << ", y is " << y << std::endl;
                  
                  response(res, "OK", action);
                  sendAll(game, action, json{
                     {"x", x},
                     {"y", y},
                     {"sender", senderID},
                  });
               }
               
               else if (action == "start-water") {
                  response(res, "OK", action);
                  sendAll(game, action, json{});
               }

            }); // game var end
         }); // games->updateJson end (games var end)
      }

   }
   
   
} // RouteGame