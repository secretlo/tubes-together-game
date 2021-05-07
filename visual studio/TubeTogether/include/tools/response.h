#pragma once

#include <iostream>
#include <string>
#include <cpp_httplib/httplib.h>
#include <json/json.hpp>
using namespace httplib;
using json = nlohmann::json;

void response(Response& res, std::string status, std::string action, json data = json::object());

#include "response.cpp"
