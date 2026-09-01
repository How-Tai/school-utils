#include <crow.h>

void initFrontend(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/")
    ([] {
        crow::response res;
        res.set_static_file_info("frontend/index.html");
        return res;
    });

    CROW_ROUTE(app, "/style.css")
    ([] {
        crow::response res;
        res.set_static_file_info("frontend/style.css");
        return res;
    });

    CROW_ROUTE(app, "/script.js")
    ([] {
        crow::response res;
        res.set_static_file_info("frontend/script.js");
        return res;
    });
}