#pragma once

#include <libpq-fe.h>
#include <memory>
#include <string>
#include <vector>

struct PgResultDeleter {
	void operator()(PGresult* result) const;
};

struct PgConnectionDeleter {
	void operator()(PGconn* connection) const;
};

using PgResult = std::unique_ptr<PGresult, PgResultDeleter>;
using PgConnection = std::unique_ptr<PGconn, PgConnectionDeleter>;

PgConnection openDatabase();
PgResult queryDatabase(PGconn* connection, const std::string& query, const std::vector<std::string>& params = {});
bool databaseResultOk(PGresult* result);
