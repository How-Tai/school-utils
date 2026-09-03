#include "attendanceMarker.h"

void registerDschoolAttendanceMarker(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/dschool/attendance")
    .methods(crow::HTTPMethod::POST)
    ([](const crow::request& req) {
        auto payload = crow::json::load(req.body);
    });
}