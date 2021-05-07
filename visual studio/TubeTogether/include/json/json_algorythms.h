#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <json/json.hpp>

void jsonForEach(nlohmann::json* data, std::function<void(nlohmann::json&)> callback);

void jsonForSome(nlohmann::json* data, std::function<bool(nlohmann::json&)> predicate, std::function<void(nlohmann::json&)> callback);
template <typename T>
void jsonForSome(nlohmann::json* data, std::string ifProp, T equalsVal, std::function<void(nlohmann::json&)> callback);

void jsonDeleteSome(nlohmann::json* data, std::function<bool(nlohmann::json&)> predicate);
template <typename T>
void jsonDeleteSome(nlohmann::json* data, std::string ifProp, T equalsVal);

#include "json_algorythms.cpp"