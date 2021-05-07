#include <iostream>
#include <string>

#include <cpp_httplib/httplib.h>
#include <json/json.hpp>
using namespace httplib;
using json = nlohmann::json;

#include <File/File.h>
#include <json/json_algorythms.h>
#include <tools/response.h>
#include <tools/getParam.h>

namespace RouteLogin {
   extern std::string pwd;
   void GetHandler(const Request& req, Response& res);
   void PostHandler(const Request& req, Response& res);
} // RouteLogin

#include "login.cpp"