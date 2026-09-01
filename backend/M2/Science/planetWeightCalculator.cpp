#include "planetWeightCalculator.h"

void registerSciPlanetWeightCalculator(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/M2/Science/planetWeightCalculator")
    .methods(crow::HTTPMethod::POST)
    ([](const crow::request& req){
        try {
            auto payload = crow::json::load(req.body);

            if(!payload) {
                return crow::response(400, "Invalid data");
            }
            
            constexpr double earthGravity = 9.81; // 9.81 m/(s^2)

            double mass;

            mass = payload["mass"].d();
            
            std::vector<double> gravity{1.00, 0.38, 0.90, 0.165, 0.38, 2.53, 1.07, 0.89, 1.14, 0.063};
            std::vector<std::string> planets{"Earth", "Mercury", "Venus", "Moon", "Mars", "Jupiter", "Saturn", "Uranus", "Neptune", "Pluto"};

            crow::json::wvalue result;

            for (size_t i = 0; i < planets.size(); ++i) {
                double weight = mass * earthGravity * gravity[i];
                result[planets[i]] = weight;
            }

            return crow::response(result);
        }
        catch(const std::exception& e) {
            return crow::response(400, e.what());
        }
    });

}