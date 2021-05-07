namespace RouteLogin {
   std::string pwd = File::pwd();
   
   void GetHandler(const Request& req, Response& res) {

   }

   void PostHandler(const Request& req, Response& res) {
      std::cout << "Login: Post" << std::endl;

      std::string login = getParam(req, "login");
      std::string password = getParam(req, "password");
      std::string type  = getParam(req, "type");
      
      File* users = new File("../data/users.json");

      auto findUser = [&](json* users, size_t* isFoundID, bool checkPassword){
         bool isFound = false;

         ::jsonForEach(users, [&](json& user){
            auto userLogin = user["login"].get<std::string>(),
               userPassword = user["password"].get<std::string>();
            if (userLogin == login && (!checkPassword || userPassword == password)) {
               isFound = true;
               *isFoundID = user["id"].get<size_t>();
            }
         });
         
         return isFound;
      };
      
      bool isFound = false;
      size_t isFoundID;

      if (type == "login") {
         std::cout << "Login: Type is login" << std::endl;

         users->updateJson([&](json* users){
            isFound = findUser(users, &isFoundID, true);
         });
         
         if (isFound) {
            std::cout << "Login: Found\n";
            response(res, "OK", "login", json{
               {"id", isFoundID},
            });
            std::cout << "Login: Login with id " << isFoundID << std::endl;
         } else {
            std::cout << "Login: Not found\n";
            response(res, "Not found", "login", json{
               {"message", "Not correct login or password"},
            });
         }
      }
      else if (type == "register") {
         std::cout << "Login: Type is register" << std::endl;

         users->updateJson([&](json* users){
            isFound = findUser(users, &isFoundID, false);
         });

         if (isFound) {
            response(res, "Exists", "register", json{
               {"message", "Login already exists"},
            });
         }
         else {
            json defaultUser = File::ReadJson("../data/default-user.json");
            defaultUser["login"] = login;
            defaultUser["password"] = password;

            size_t maxID = 0;

            users->updateJson([&](json* users){
               ::jsonForEach(users, [&](json& user){
                  size_t userID = user["id"].get<size_t>();
                  if (userID > maxID) maxID = userID;
               });
               defaultUser["id"] = maxID + 1;

               users->push_back(defaultUser);
            });
            
            response(res, "OK", "register", json{
               {"id", maxID + 1},
            });

            std::cout << "Login: Register with id " << maxID + 1 << std::endl;
         }
      }

      else if (type == "levels") {
         std::cout << "Login: Type is levels" << std::endl;

         size_t id = std::stoi(getParam(req, "id"));
         size_t levels;

         users->updateJson([&](json* users){
            jsonForSome(users, "id", id, [&](json& user){
               levels = user["levels"].get<size_t>();
            });
         });

         response(res, "OK", "levels", json{
            {"levels", levels},
         });
      }
   }
   
} // RouteLogin