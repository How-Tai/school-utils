#include <crow.h>

void registerFrontend(crow::SimpleApp& app) {

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





        CROW_ROUTE(app, "/M2/Math/quadratic-solver")
    ([] {
        crow::response res;
        res.set_static_file_info("frontend/M2/Math/quadratic-solver/index.html");
        return res;
    });

    CROW_ROUTE(app, "/M2/Math/quadratic-solver/script.js")
    ([] {
        crow::response res;
        res.set_static_file_info("frontend/M2/Math/quadratic-solver/script.js");
        return res;
    });

    CROW_ROUTE(app, "/M2/Math/quadratic-solver/style.css")
    ([] {
        crow::response res;
        res.set_static_file_info("frontend/M2/Math/quadratic-solver/style.css");
        return res;
    });





    CROW_ROUTE(app, "/M2/Science/planet-weight")
    ([] {
        crow::response res;
        res.set_static_file_info("frontend/M2/Science/planet-weight/index.html");
        return res;
    });

    CROW_ROUTE(app, "/M2/Science/planet-weight/script.js")
    ([] {
        crow::response res;
        res.set_static_file_info("frontend/M2/Science/planet-weight/script.js");
        return res;
    });

    CROW_ROUTE(app, "/M2/Science/planet-weight/style.css")
    ([] {
        crow::response res;
        res.set_static_file_info("frontend/M2/Science/planet-weight/style.css");
        return res;
    });





    CROW_ROUTE(app, "/M2/Science/sig-fig-counting")
    ([] {
        crow::response res;
        res.set_static_file_info("frontend/M2/Science/sig-fig-counting/index.html");
        return res;
    });

    CROW_ROUTE(app, "/M2/Science/sig-fig-counting/script.js")
    ([] {
        crow::response res;
        res.set_static_file_info("frontend/M2/Science/sig-fig-counting/script.js");
        return res;
    });

    CROW_ROUTE(app, "/M2/Science/sig-fig-counting/style.css")
    ([] {
        crow::response res;
        res.set_static_file_info("frontend/M2/Science/sig-fig-counting/style.css");
        return res;
    });
}
