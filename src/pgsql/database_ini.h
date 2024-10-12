#ifndef DATABASE_INI_H
#define DATABASE_INI_H

#include "libpq-fe.h"
#include <iostream>
#include <numeric>
#include <string>

namespace pgsqlInitialization {

	class DatabaseInitializer {
	 public:
		// Constructor that takes superuser credentials for database management
		DatabaseInitializer(const std::string& superUserName = "postgres", const std::string& superUserPassword = "");

		// Destructor to close any open connections
		~DatabaseInitializer();

		// Method to initialize tables in the database
		bool initializeTables();

		// Method to create a new user and database
		bool createUserAndDatabase(const std::string& dbName, const std::string& userName, const std::string& password);

		// Method to check if a specific database exists
		bool checkDatabaseExists(const std::string& dbName);

	 private:
		PGconn* conn_; // PostgreSQL connection object
		std::string superUserConnInfo_; // Connection string for superuser
	};

	void pgsqlInitializationManagementMenu();

} // namespace pgsqlInitialization

#endif // DATABASE_INI_H
