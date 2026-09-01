#include "crow.h"

int main() {
  crow::SimpleApp app;
  
  CROW_ROUTE(app, "api/placeholder")
  ([] {
    crow::json::wvalve res;
    res["msg"] = "test";
    return res;
  });

  app.port(3000).multithreaded().run();
}
