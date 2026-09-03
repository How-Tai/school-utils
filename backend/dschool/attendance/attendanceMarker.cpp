#include "attendanceMarker.h"

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"

std::pair<double, double> randomPointInRectangle() {
	double lat1 = 16.4316281;
	double lon1 = 102.8343359;

	double lat2 = 16.4325587;
	double lon2 = 102.8356737;

	static std::random_device rd;
	static std::mt19937 gen(rd());

	std::uniform_real_distribution<double> latDist(std::min(lat1, lat2), std::max(lat1, lat2));
	std::uniform_real_distribution<double> lonDist(std::min(lon1, lon2), std::max(lon1, lon2));

	return {latDist(gen), lonDist(gen)};
}

void registerDschoolAttendanceMarker(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/dschool/attendance")
    .methods(crow::HTTPMethod::POST)
    ([](const crow::request& req) {
        auto payload = crow::json::load(req.body);

        if(!payload) {
            return crow::response(400, "Invalid data");
        }

        httplib::SSLClient cli("dschool-g7w.gp-education.com");
        
    });
}
