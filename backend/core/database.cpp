#include "database.h"

#include <cstdlib>
#include <stdexcept>

void PgResultDeleter::operator()(PGresult* result) const {
	if(result) PQclear(result);
}

void PgConnectionDeleter::operator()(PGconn* connection) const {
	if(connection) PQfinish(connection);
}

PgConnection openDatabase() {
	const char* url = std::getenv("DATABASE_URL");
	if(!url) throw std::runtime_error("DATABASE_URL is not configured");

	PgConnection connection(PQconnectdb(url));
	if(!connection || PQstatus(connection.get()) != CONNECTION_OK) throw std::runtime_error("Could not connect to database");
	return connection;
}

PgResult queryDatabase(PGconn* connection, const std::string& query, const std::vector<std::string>& params) {
	std::vector<const char*> values;
	values.reserve(params.size());
	for(const auto& param : params) values.push_back(param.c_str());

	return PgResult(PQexecParams(connection, query.c_str(), static_cast<int>(params.size()), nullptr, values.data(), nullptr, nullptr, 0));
}

bool databaseResultOk(PGresult* result) {
	if(!result) return false;
	auto status = PQresultStatus(result);
	return status == PGRES_COMMAND_OK || status == PGRES_TUPLES_OK;
}
