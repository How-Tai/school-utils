#pragma once

#include <crow.h>
#include <string>

struct SessionUser {
	long long id = 0;
	std::string username;
	std::string displayName;
	std::string role;
	bool valid = false;
};

SessionUser currentUser(const crow::request& req);
bool isAdmin(const SessionUser& user);
void registerAccount(crow::SimpleApp& app);
