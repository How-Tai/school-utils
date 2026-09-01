#include "crow.h"
#include "paths.h"

int main() {
  crow::SimpleApp app;
  
  initServer(app);

  app.port(3000).multithreaded().run();
}
