#include "create-game.h"

namespace RouteCreateGame {
   std::string pwd = File::pwd();

   size_t getMinId(json games) {
      if (games.size() == 0) return 0;

      size_t prevId = 0;

      for (auto& game : games) {
         size_t id = game["id"].get<size_t>();
         if (id - prevId > 1) return prevId + 1;
         prevId = id;
      }
      
      return prevId + 1;
   }
   
   void GetHandler(const Request& req, Response& res) {

   }

   void PostHandler(const Request& req, Response& res) {
      std::cout << "Create Game: Post\n";

      auto gamesFile = new File(pwd + "/../data/games.json");

      json games = gamesFile->readJson();
      json gameTpl = File::ReadJson(pwd + "/../data/default-game.json");
      
      int gameId = getMinId(games);
      
      // Updating default values
      gameTpl["id"] = gameId;
      games.push_back(gameTpl);

      gamesFile->writeJson(games);
      
      res.set_content("{ \"status\": \"OK\", \"session_id\": " + std::to_string(gameId) + "}", "text/json; charset=UTF-8");
      std::cout << "Create Game: responded with gameId " << gameId << std::endl;
   }
   
} // RouteCreateGame
