#include <iostream>
#include <string>

#include <cpp_httplib/httplib.h>
#include <json/json.hpp>
using namespace httplib;
using json = nlohmann::json;

#include <File/File.h>
#include <tools/getParam.h>
#include <tools/response.h>
#include <json/json_algorythms.h>

namespace RouteGame {
   extern std::string pwd;

   int randint(int min, int max);
   void sendAll(json& game, std::string action, json data);

   size_t getReadyCount(json& game);
   size_t getConnectionCount(json& game);
   
   void PostHandler(const Request& req, Response& res);
   
} // RouteGame

#include "game.cpp"