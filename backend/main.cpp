#include <crow.h>
#include <cstdlib>
#include <string>

#include "paths.h"

int main() {
    crow::SimpleApp app;

    initServer(app);

    int port = 80;

    if (const char* env = std::getenv("PORT")) {
        port = std::stoi(env);
    }

    app.bindaddr("0.0.0.0")
       .port(port)
       .multithreaded()
       .run();
}
