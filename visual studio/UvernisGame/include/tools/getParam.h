#pragma once

#include <iostream>
#include <string>
#include <cpp_httplib/httplib.h>
using namespace httplib;

std::string getParam(const Request& req, std::string paramName);

#include "getParam.cpp"