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
                  defaultGameUser = File::ReadJson(pwd + "/../data/default-game-user.json");
               
               size_t gameID;
               
               games->updateJson([&](json* games){
                  defaultGameUser["id"] = user->at("id").get<size_t>();
                  defaultGameUser["host"] = user->at("host").get<std::string>();
                  defaultGame["users"].push_back(defaultGameUser);

                  defaultGameUser["id"] = partner->at("id").get<size_t>();
                  defaultGameUser["host"] = partner->at("host").get<std::string>();
                  defaultGame["users"].push_back(defaultGameUser);
                  
                  size_t maxID = 0;
                  jsonForEach(games, [&](json& game){
                     size_t gameID = game["id"].get<size_t>();
                     if (maxID < gameID) maxID = gameID;
                  });
                  gameID = maxID + 1;
                  defaultGame["id"] = gameID;
                  
                  games->push_back(defaultGame);

                  jsonForSome(games, "id", gameID, [&](json& game){
                     sendAll(game, "start", json{
                        {"seesion_id", gameID},
                        {"players", game["users"]},
                     });
                     response(res, "Connected", action);
                  });
               });
            } // if partner
            else { // else if NO partner
               user->at("isWait") = true;
               user->at("host") = host;
               //response(res, "Wait", action);
               response(res, "Connected", action);
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
      }

   }
   
   
} // RouteGame