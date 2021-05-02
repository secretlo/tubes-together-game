#include <iostream>
#include <string>
#include <vector>
#include <algorithm> // for erase-remove idiom

#include <cpp_httplib/httplib.h>
using namespace httplib;
#include <json/json.hpp>
using json = nlohmann::json;

#include <File/File.h>
#include <tools/getParam.h>
#include <json/json_algorythms.h>

namespace RouteGame {
   extern std::string pwd;
   
   void response(Response& res, std::string action, json data);
   int randint(int min, int max);
   void sendAll(json& game, std::string action, json data);

   size_t getReadyCount(json& game);
   size_t getConnectionCount(json& game);
   
   void PostHandler(const Request& req, Response& res);
   
} // RouteGame

#include "game.cpp"