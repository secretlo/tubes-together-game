#include "response.h"

void response(Response& res, std::string status, std::string action, json data /* = {} */) {
   data["action"] = action;
   data["status"] = status;
   res.set_content(data.dump().c_str(), "text/json; charset=UTF-8");
   std::cout << "response: responded with data " << data.dump(3) << "\n";
}