#include "File.h"

// static

std::string File::pwd() {
#ifdef _WIN32
   // Windows
   TCHAR path[MAX_PATH];
   GetCurrentDirectory(MAX_PATH, path);
   return std::string(path);
#else
   // Linux
   char result[PATH_MAX];
   ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
   const char* path;
   if (count != -1) {
      path = dirname(result);
   }
   return std::string(path);
#endif
}

std::string File::Read(std::string path) {
   File* f = new File(path);
   return f->read();
}
void File::Write(std::string path, std::string data) {
   File* f = new File(path);
   f->write(data);
}

json File::ReadJson(std::string path) {
   File* f = new File(path);
   return f->readJson();
}
void File::WriteJson(std::string path, json data) {
   File* f = new File(path);
   f->writeJson(data);
}


// constructors

File::File(std::string abspathToFile) {
   abspath = abspathToFile;
}


// methods

std::string File::read() {
   std::string res;
   std::ifstream file(this->abspath);
   if (file.good()) {
      res = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
      file.close();
      return res;
   }
   else {
      file.close();
      std::string message = "File::read Error: file ifstream is bad (" + this->abspath + ")";
      std::cout << message << std::endl;
      throw message;
   }
}
void File::write(const std::string& data) {
   std::ofstream file;
   file.open(abspath);
   file << data;
   file.close();
}

json File::readJson() {
   try {
      return json::parse(read());
   } catch (...) {
      std::cout << "Error in File::readJson: Failed to read or json::parse data\n";
   }
}
void File::writeJson(json data) {
   write(data.dump(File::indent));
}

bool File::exists() {
   std::ifstream file(abspath);
   bool res = file.good();
   file.close();
   return res;
}

void File::update(std::function<std::string(std::string)> callback) {
   std::string res = callback(this->read());
   this->write(res);
}

void File::updateJson(std::function<void(json*)> callback) {
   json res = this->readJson();
   callback(&res);
   this->writeJson(res);
}

void File::updateChilds(std::string childProp, std::string childVal, std::function<void(json&)> callback) {
   json content = this->readJson();
   for (auto& child : content) {
      if (child.contains(childProp)) {
         std::string currVal = child[childProp].get<std::string>();
         if (currVal == childVal)
            callback(child);
      }
   }
   this->writeJson(content);
}
void File::updateChilds(std::string childProp, size_t childVal, std::function<void(json&)> callback) {
   json content = this->readJson();
   for (auto& child : content) {
      if (child.contains(childProp)) {
         size_t currVal = child[childProp].get<size_t>();
         if (currVal == childVal)
            callback(child);
      }
   }
   this->writeJson(content);
}
void File::updateChilds(std::function<bool(json&)> predicate, std::function<void(json&)> callback) {
   json content = this->readJson();
   for (auto& child : content) {
      if (predicate(child))
         callback(child);
   }
   this->writeJson(content);
}

void File::forEach(std::function<void(json&)> callback) {
   json content = this->readJson();
   for (auto& child : content) {
      callback(child);
   }
}

void File::pushChild(json child) {
   this->updateJson([&](json* content){
      content->push_back(child);
   });
}

void File::deleteChild(std::string childProp, std::string childVal) {
   json content = this->readJson();
   
   size_t i = 0;
   bool wasFound = true;

   for (auto it = content.begin(); it != content.end(); it++, i++) {
      if ((*it).contains(childProp))
         if ((*it).at(childProp).get<std::string>() == childVal) {
            wasFound = true;
            break;
         }
   }
   
   if (wasFound)
      content.erase(content.begin() + i);
}