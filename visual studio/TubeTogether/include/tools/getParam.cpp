std::string getParam(const Request& req, std::string paramName) {
   if (!req.has_param(paramName.c_str())) {
      std::cout << "Error getParam: Trying to get param \"" + paramName + "\", but no such param in req\n";
      throw "Error getParam: Trying to get param \"" + paramName + "\", but no such param in req\n";
   }
   
   return req.get_param_value(paramName.c_str());
}