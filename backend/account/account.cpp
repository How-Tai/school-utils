#include "account.h"

#include "../core/database.h"
#include "../core/security.h"
#include <algorithm>
#include <cctype>

namespace {
	crow::response jsonError(int code, const std::string& message) {
		crow::json::wvalue body;
		body["error"] = message;
		crow::response res(code, body);
		res.set_header("Content-Type", "application/json");
		return res;
	}

	bool validUsername(const std::string& username) {
		if(username.size() < 3 || username.size() > 24) return false;
		return std::all_of(username.begin(), username.end(), [](unsigned char c) { return std::isalnum(c) || c == '_' || c == '-'; });
	}

	crow::response createSession(long long userId) {
		std::string token = randomHex(32);
		if(token.empty()) return jsonError(500, "Could not create session");

		try {
			auto db = openDatabase();
			auto result = queryDatabase(db.get(), "INSERT INTO sessions(user_id, token_hash, expires_at) VALUES($1, $2, NOW() + INTERVAL '8 hours')", {std::to_string(userId), sha256Hex(token)});
			if(!databaseResultOk(result.get())) return jsonError(500, "Could not create session");
		}
		catch(...) {
			return jsonError(500, "Database unavailable");
		}

		crow::json::wvalue body;
		body["ok"] = true;
		crow::response res(200, body);
		res.add_header("Set-Cookie", "session=" + token + "; Path=/; HttpOnly; Secure; SameSite=Lax; Max-Age=28800");
		res.set_header("Cache-Control", "no-store");
		return res;
	}
}

SessionUser currentUser(const crow::request& req) {
	SessionUser user;
	std::string token = getCookie(req.get_header_value("Cookie"), "session");
	if(token.empty()) return user;

	try {
		auto db = openDatabase();
		auto result = queryDatabase(db.get(), "SELECT u.id, u.username, u.display_name, u.role FROM sessions s JOIN users u ON u.id = s.user_id WHERE s.token_hash = $1 AND s.expires_at > NOW() LIMIT 1", {sha256Hex(token)});
		if(!databaseResultOk(result.get()) || PQntuples(result.get()) != 1) return user;

		user.id = std::stoll(PQgetvalue(result.get(), 0, 0));
		user.username = PQgetvalue(result.get(), 0, 1);
		user.displayName = PQgetvalue(result.get(), 0, 2);
		user.role = PQgetvalue(result.get(), 0, 3);
		user.valid = true;
	}
	catch(...) {}
	return user;
}

bool isAdmin(const SessionUser& user) {
	return user.valid && (user.role == "admin" || user.role == "owner");
}

void registerAccount(crow::SimpleApp& app) {
	CROW_ROUTE(app, "/api/account/signup").methods(crow::HTTPMethod::POST)([](const crow::request& req) {
		auto body = crow::json::load(req.body);
		if(!body || !body.has("username") || !body.has("displayName") || !body.has("password")) return jsonError(400, "Missing fields");

		std::string username = body["username"].s();
		std::string displayName = body["displayName"].s();
		std::string password = body["password"].s();
		if(!validUsername(username)) return jsonError(400, "Username must be 3-24 letters, numbers, _ or -");
		if(displayName.empty() || displayName.size() > 40) return jsonError(400, "Invalid display name");
		if(password.size() < 10 || password.size() > 128) return jsonError(400, "Password must be 10-128 characters");

		std::string salt = randomHex(16);
		std::string hash = passwordHash(password, salt);
		if(salt.empty() || hash.empty()) return jsonError(500, "Could not secure password");

		try {
			auto db = openDatabase();
			auto result = queryDatabase(db.get(), "INSERT INTO users(username, display_name, password_salt, password_hash, role) VALUES($1, $2, $3, $4, 'user') RETURNING id", {username, displayName, salt, hash});
			if(!databaseResultOk(result.get())) {
				if(std::string(PQresultErrorField(result.get(), PG_DIAG_SQLSTATE) ? PQresultErrorField(result.get(), PG_DIAG_SQLSTATE) : "") == "23505") return jsonError(409, "Username already exists");
				return jsonError(500, "Could not create account");
			}
			return createSession(std::stoll(PQgetvalue(result.get(), 0, 0)));
		}
		catch(...) {
			return jsonError(500, "Database unavailable");
		}
	});

	CROW_ROUTE(app, "/api/account/login").methods(crow::HTTPMethod::POST)([](const crow::request& req) {
		auto body = crow::json::load(req.body);
		if(!body || !body.has("username") || !body.has("password")) return jsonError(400, "Missing fields");
		std::string username = body["username"].s();
		std::string password = body["password"].s();
		if(username.size() > 24 || password.size() > 128) return jsonError(401, "Invalid username or password");

		try {
			auto db = openDatabase();
			auto result = queryDatabase(db.get(), "SELECT id, password_salt, password_hash FROM users WHERE username = $1 LIMIT 1", {username});
			if(!databaseResultOk(result.get()) || PQntuples(result.get()) != 1) return jsonError(401, "Invalid username or password");

			std::string salt = PQgetvalue(result.get(), 0, 1);
			std::string storedHash = PQgetvalue(result.get(), 0, 2);
			if(!constantTimeEqual(passwordHash(password, salt), storedHash)) return jsonError(401, "Invalid username or password");
			return createSession(std::stoll(PQgetvalue(result.get(), 0, 0)));
		}
		catch(...) {
			return jsonError(500, "Database unavailable");
		}
	});

	CROW_ROUTE(app, "/api/account/logout").methods(crow::HTTPMethod::POST)([](const crow::request& req) {
		std::string token = getCookie(req.get_header_value("Cookie"), "session");
		if(!token.empty()) {
			try {
				auto db = openDatabase();
				queryDatabase(db.get(), "DELETE FROM sessions WHERE token_hash = $1", {sha256Hex(token)});
			}
			catch(...) {}
		}
		crow::json::wvalue body;
		body["ok"] = true;
		crow::response res(200, body);
		res.add_header("Set-Cookie", "session=; Path=/; HttpOnly; Secure; SameSite=Lax; Max-Age=0");
		return res;
	});

	CROW_ROUTE(app, "/api/account/me").methods(crow::HTTPMethod::GET)([](const crow::request& req) {
		auto user = currentUser(req);
		crow::json::wvalue body;
		body["loggedIn"] = user.valid;
		if(user.valid) {
			body["username"] = user.username;
			body["displayName"] = user.displayName;
			body["role"] = user.role;
		}
		crow::response res(200, body);
		res.set_header("Cache-Control", "no-store");
		return res;
	});
}
