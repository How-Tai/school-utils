#include "community.h"

#include "../account/account.h"
#include "../core/database.h"
#include <cctype>

namespace {
	crow::response errorResponse(int code, const std::string& message) {
		crow::json::wvalue body;
		body["error"] = message;
		return crow::response(code, body);
	}

	std::string trim(const std::string& value) {
		size_t l = 0, r = value.size();
		while(l < r && std::isspace(static_cast<unsigned char>(value[l]))) ++l;
		while(r > l && std::isspace(static_cast<unsigned char>(value[r - 1]))) --r;
		return value.substr(l, r - l);
	}
}

void registerCommunity(crow::SimpleApp& app) {
	CROW_ROUTE(app, "/api/announcements").methods(crow::HTTPMethod::GET)([] {
		try {
			auto db = openDatabase();
			auto result = queryDatabase(db.get(), "SELECT id, title, message, created_at::text FROM announcements ORDER BY created_at DESC LIMIT 20");
			if(!databaseResultOk(result.get())) return errorResponse(500, "Could not load announcements");

			crow::json::wvalue body;
			body["announcements"] = crow::json::wvalue::list();
			for(int i = 0; i < PQntuples(result.get()); ++i) {
				crow::json::wvalue item;
				item["id"] = std::stoll(PQgetvalue(result.get(), i, 0));
				item["title"] = PQgetvalue(result.get(), i, 1);
				item["message"] = PQgetvalue(result.get(), i, 2);
				item["createdAt"] = PQgetvalue(result.get(), i, 3);
				body["announcements"][i] = std::move(item);
			}
			crow::response res(200, body);
			res.set_header("Cache-Control", "no-store");
			return res;
		}
		catch(...) {
			return errorResponse(500, "Database unavailable");
		}
	});

	CROW_ROUTE(app, "/api/feedback").methods(crow::HTTPMethod::POST)([](const crow::request& req) {
		auto body = crow::json::load(req.body);
		if(!body || !body.has("type") || !body.has("message")) return errorResponse(400, "Missing fields");
		std::string type = body["type"].s();
		std::string message = body["message"].s();
		if(type != "complaint" && type != "suggestion") return errorResponse(400, "Invalid feedback type");
		if(message.size() < 3 || message.size() > 4000) return errorResponse(400, "Message must be 3-4000 characters");

		try {
			auto db = openDatabase();
			auto result = queryDatabase(db.get(), "INSERT INTO feedback(type, message) VALUES($1, $2)", {type, message});
			if(!databaseResultOk(result.get())) return errorResponse(500, "Could not submit feedback");
			crow::json::wvalue response;
			response["ok"] = true;
			return crow::response(201, response);
		}
		catch(...) {
			return errorResponse(500, "Database unavailable");
		}
	});

	CROW_ROUTE(app, "/api/admin/feedback").methods(crow::HTTPMethod::GET)([](const crow::request& req) {
		if(!isAdmin(currentUser(req))) return errorResponse(403, "Admin access required");
		try {
			auto db = openDatabase();
			auto result = queryDatabase(db.get(), "SELECT id, type, message, created_at::text FROM feedback ORDER BY created_at DESC LIMIT 200");
			if(!databaseResultOk(result.get())) return errorResponse(500, "Could not load feedback");

			crow::json::wvalue body;
			body["feedback"] = crow::json::wvalue::list();
			for(int i = 0; i < PQntuples(result.get()); ++i) {
				crow::json::wvalue item;
				item["id"] = std::stoll(PQgetvalue(result.get(), i, 0));
				item["type"] = PQgetvalue(result.get(), i, 1);
				item["message"] = PQgetvalue(result.get(), i, 2);
				item["createdAt"] = PQgetvalue(result.get(), i, 3);
				body["feedback"][i] = std::move(item);
			}
			return crow::response(200, body);
		}
		catch(...) {
			return errorResponse(500, "Database unavailable");
		}
	});

	CROW_ROUTE(app, "/api/admin/feedback/<int>").methods(crow::HTTPMethod::DELETE)([](const crow::request& req, int id) {
		if(!isAdmin(currentUser(req))) return errorResponse(403, "Admin access required");
		if(id <= 0) return errorResponse(400, "Invalid feedback id");
		try {
			auto db = openDatabase();
			auto result = queryDatabase(db.get(), "DELETE FROM feedback WHERE id = $1", {std::to_string(id)});
			if(!databaseResultOk(result.get())) return errorResponse(500, "Could not delete feedback");
			crow::json::wvalue response;
			response["ok"] = true;
			return crow::response(200, response);
		}
		catch(...) {
			return errorResponse(500, "Database unavailable");
		}
	});

	CROW_ROUTE(app, "/api/admin/announcements").methods(crow::HTTPMethod::GET)([](const crow::request& req) {
		if(!isAdmin(currentUser(req))) return errorResponse(403, "Admin access required");
		try {
			auto db = openDatabase();
			auto result = queryDatabase(db.get(), "SELECT id, title, message, created_at::text FROM announcements ORDER BY created_at DESC LIMIT 20");
			if(!databaseResultOk(result.get())) return errorResponse(500, "Could not load announcements");

			crow::json::wvalue body;
			body["announcements"] = crow::json::wvalue::list();
			for(int i = 0; i < PQntuples(result.get()); ++i) {
				crow::json::wvalue item;
				item["id"] = std::stoll(PQgetvalue(result.get(), i, 0));
				item["title"] = PQgetvalue(result.get(), i, 1);
				item["message"] = PQgetvalue(result.get(), i, 2);
				item["createdAt"] = PQgetvalue(result.get(), i, 3);
				body["announcements"][i] = std::move(item);
			}
			return crow::response(200, body);
		}
		catch(...) {
			return errorResponse(500, "Database unavailable");
		}
	});

	CROW_ROUTE(app, "/api/admin/announcements").methods(crow::HTTPMethod::POST)([](const crow::request& req) {
		if(!isAdmin(currentUser(req))) return errorResponse(403, "Admin access required");
		auto body = crow::json::load(req.body);
		if(!body || !body.has("title") || !body.has("message")) return errorResponse(400, "Missing fields");
		std::string title = trim(body["title"].s());
		std::string message = trim(body["message"].s());
		if(title.empty() || message.empty()) return errorResponse(400, "Title and message are required");
		if(title.size() > 120 || message.size() > 2000) return errorResponse(400, "Announcement is too long");
		try {
			auto db = openDatabase();
			auto result = queryDatabase(db.get(), "INSERT INTO announcements(title, message) VALUES($1, $2)", {title, message});
			if(!databaseResultOk(result.get())) return errorResponse(500, "Could not create announcement");
			crow::json::wvalue response;
			response["ok"] = true;
			return crow::response(201, response);
		}
		catch(...) {
			return errorResponse(500, "Database unavailable");
		}
	});

	CROW_ROUTE(app, "/api/admin/announcements/<int>").methods(crow::HTTPMethod::DELETE)([](const crow::request& req, int id) {
		if(!isAdmin(currentUser(req))) return errorResponse(403, "Admin access required");
		if(id <= 0) return errorResponse(400, "Invalid announcement id");
		try {
			auto db = openDatabase();
			auto result = queryDatabase(db.get(), "DELETE FROM announcements WHERE id = $1", {std::to_string(id)});
			if(!databaseResultOk(result.get())) return errorResponse(500, "Could not delete announcement");
			crow::json::wvalue response;
			response["ok"] = true;
			return crow::response(200, response);
		}
		catch(...) {
			return errorResponse(500, "Database unavailable");
		}
	});
}
