void jsonForEach(nlohmann::json* data, std::function<void(nlohmann::json&)> callback) {
   for (auto& it : *data) {
      callback(it);
   }
}

void jsonForSome(nlohmann::json* data, std::function<bool(nlohmann::json&)> predicate, std::function<void(nlohmann::json&)> callback) {
   for (auto& it : *data) {
      if (predicate(it))
         callback(it);
   }
}

template <typename T>
void jsonForSome(nlohmann::json* data, std::string ifProp, T equalsVal, std::function<void(nlohmann::json&)> callback) {
   for (auto& it : *data) {
      if (it[ifProp].get<T>() == equalsVal)
         callback(it);
   }
}

void jsonDeleteSome(nlohmann::json* data, std::function<bool(nlohmann::json&)> predicate) {
   std::vector<size_t> toDelete;
   size_t i = 0;
   for (auto& it : *data) {
      if (predicate(it))
         toDelete.push_back(i);
      i++;
   }
   for (std::vector<size_t>::reverse_iterator i = toDelete.rbegin(); i != toDelete.rend(); ++i) {
      data->erase(data->begin() + (*i));
   }
}

template <typename T>
void jsonDeleteSome(nlohmann::json* data, std::string ifProp, T equalsVal) {
   std::vector<size_t> toDelete;
   size_t i = 0;
   for (auto& it : *data) {
      if (it[ifProp].get<T>() == equalsVal)
         toDelete.push_back(i);
      i++;
   }
   for (std::vector<size_t>::reverse_iterator i = toDelete.rbegin(); i != toDelete.rend(); ++i) {
      data->erase(data->begin() + (*i));
   }
}