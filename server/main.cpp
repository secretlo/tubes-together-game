#include <iostream>
#include <string>
#include <File/File.h>
#include <cpp_httplib/httplib.h>
#include <json/json.hpp>

using namespace httplib;
using json = nlohmann::json;

Server server;

const char* SERVER_HOST = "127.0.0.1";
const int SERVER_PORT = 1234;

#include "routes/root.h"
#include "routes/login.h"
#include "routes/create-game.h"
#include "routes/game.h"

int File::indent = 3;


int main() {

   server
      .Post("/", RouteRoot::Handler)
      .Post("/login", RouteLogin::PostHandler)
      .Post("/create-game", RouteCreateGame::PostHandler)
      .Post("/game", RouteGame::PostHandler);


   // Starting server
   std::cout << "Server started on " << SERVER_HOST << ":" << SERVER_PORT << "\n";
   server.listen(SERVER_HOST, SERVER_PORT);

   return 0;
}