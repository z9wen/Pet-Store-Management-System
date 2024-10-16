#ifndef PGSQL_SUPERUSER_H
#define PGSQL_SUPERUSER_H

#include "libpq-fe.h"
#include <iostream>
#include <string>

namespace pgsqlSuperUser {

	// PgSQLSuperUserManager class to manage PostgreSQL superuser operations
	class PgSQLSuperUserManager {
	 public:
		// Constructor: initializes the PostgreSQL connection info
		PgSQLSuperUserManager(const std::string& superUserName, const std::string& superUserPassword);

		// Destructor: destroys the connection
		~PgSQLSuperUserManager();

		// Connects to the PostgreSQL database
		bool connect();

		// Lists all the superusers
		void listSuperUsers() const;

		// Checks if a given user is a superuser
		bool isUserSuperUser(const std::string& superUserName) const;

		// Creates a new superuser
		bool createSuperUser(const std::string& superUserName, const std::string& password) const;

		// Drop superuser
		bool dropSuperUser(const std::string& superUserName) const;

	 private:
		PGconn* conn_ = nullptr; // PostgreSQL connection object
		std::string connInfo_; // Connection string
	};

	// Displays the superuser management menu
	void sqlSuperUsersManagementMenuShow();

	// Gets the superuser connection info (username and password)
	std::string getSuperUserConnectionInfo(std::string& superUserName, std::string& superUserPassword);

	// Handles menu operations for superuser management
	void sqlSuperUsersManagementMenu();

} // namespace pgsqlSuperUser

#endif // PGSQL_SUPERUSER_H
