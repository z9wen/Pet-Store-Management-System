#include "pgsql_superuser.h"

#include "libpq-fe.h"
#include <iostream>
#include <numeric>
#include <string>

namespace pgsqlSuperUser {
	// Constructor: Initializes the connection string
	PgSQLSuperUserManager::PgSQLSuperUserManager(const std::string& superUserName, const std::string& superUserPassword)
	: connInfo_("dbname=postgres user=" + superUserName + " host=localhost port=5432"
	            + (superUserPassword.empty() ? "" : " password=" + superUserPassword)) {}

	// Destructor: Closes the connection if it's open
	PgSQLSuperUserManager::~PgSQLSuperUserManager() {
		if (conn_) {
			PQfinish(conn_);
		}
	}

	// Connects to the PostgreSQL database using the stored connection info
	bool PgSQLSuperUserManager::connect() {
		conn_ = PQconnectdb(connInfo_.c_str());
		if (PQstatus(conn_) != CONNECTION_OK) {
			std::cerr << "Connection to database failed: " << PQerrorMessage(conn_) << std::endl;
			PQfinish(conn_);
			conn_ = nullptr; // Reset connection pointer after failed connection
			return false;
		}
		return true;
	}

	// Lists all superusers in the PostgreSQL instance
	void PgSQLSuperUserManager::listSuperUsers() const {
		const char* query = "SELECT rolname FROM pg_roles WHERE rolsuper = true;";
		PGresult* res = PQexec(conn_, query);

		if (PQresultStatus(res) != PGRES_TUPLES_OK) {
			std::cerr << "Failed to retrieve superusers: " << PQerrorMessage(conn_) << std::endl;

			PQclear(res);
			return;
		}

		int rows = PQntuples(res);
		if (rows == 0) {
			std::cout << "No superusers found." << std::endl;
		}
		else {
			std::cout << "Superusers list:" << std::endl;
			for (int i = 0; i < rows; ++i) {
				std::cout << "- " << PQgetvalue(res, i, 0) << std::endl;
			}
		}

		PQclear(res);
	}

	// Checks if a specific user is a superuser
	bool PgSQLSuperUserManager::isUserSuperUser(const std::string& superUserName) const {
		std::string query = "SELECT rolname FROM pg_roles WHERE rolname = '" + superUserName + "' AND rolsuper = true;";
		PGresult* res = PQexec(conn_, query.c_str());

		if (PQresultStatus(res) != PGRES_TUPLES_OK) {
			std::cerr << "Failed to check if user is superuser: " << PQerrorMessage(conn_) << std::endl;
			PQclear(res);
			return false;
		}

		bool isSuperUser = PQntuples(res) > 0;
		PQclear(res);
		return isSuperUser;
	}

	// Creates a new superuser
	bool PgSQLSuperUserManager::createSuperUser(const std::string& superUserName, const std::string& password) {
		std::string createUserSQL = "CREATE USER " + superUserName + " WITH SUPERUSER CREATEDB CREATEROLE LOGIN";
		if (!password.empty()) {
			createUserSQL += " PASSWORD '" + password + "';";
		}
		else {
			createUserSQL += ";";
		}

		PGresult* res = PQexec(conn_, createUserSQL.c_str());
		if (PQresultStatus(res) != PGRES_COMMAND_OK) {
			std::cerr << "Failed to create superuser: " << PQerrorMessage(conn_) << std::endl;
			PQclear(res);
			return false;
		}

		PQclear(res);
		std::cout << "Superuser created successfully." << std::endl;
		return true;
	}

	void sqlSuperUsersManagementMenuShow() {
		std::cout << "==== PostgreSQL Superuser Management ====" << std::endl;
		std::cout << "1. Check if Superuser Exists" << std::endl;
		std::cout << "2. Create Superuser" << std::endl;
		std::cout << "3. List All Superusers" << std::endl;
		std::cout << "4. Exit" << std::endl;
		std::cout << "=========================================" << std::endl;
		std::cout << "Enter your choice: ";
	}

	std::string getSuperUserConnectionInfo(std::string& superUserName, std::string& superUserPassword) {
		std::cout << "Enter superuser name (default is 'postgres'): ";
		std::getline(std::cin, superUserName);
		if (superUserName.empty()) {
			superUserName = "postgres"; // default pgsql user postgres
		}

		std::cout << "Enter superuser password (leave blank for no password): ";
		std::getline(std::cin, superUserPassword);

		std::string conninfo = "dbname=postgres user=" + superUserName;
		if (!superUserPassword.empty()) {
			conninfo += " password=" + superUserPassword;
		}
		conninfo += " host=localhost port=5432";

		return conninfo;
	}

	void sqlSuperUsersManagementMenu() {
		int choice;
		std::string superUserName;
		std::string password;

		// Get the superuser connection information
		std::string connectionSuperUser;
		std::string connectionPassword;
		std::string conninfo = getSuperUserConnectionInfo(connectionSuperUser, connectionPassword);

		// Initialize the PgSQLSuperUserManager instance
		PgSQLSuperUserManager manager(connectionSuperUser, connectionPassword);

		// Connect once and reuse the connection
		if (!manager.connect()) {
			std::cerr << "Failed to connect to the database." << std::endl;
			return;
		}

		while (true) {
			sqlSuperUsersManagementMenuShow(); // show menu
			std::cin >> choice;

			if (std::cin.fail()) {
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cerr << "Invalid input. Please enter a number." << std::endl;
				continue;
			}

			switch (choice) {
			case 1: {
				std::cout << "Enter the superuser name to check: ";
				std::cin >> superUserName;

				if (manager.isUserSuperUser(superUserName)) {
					std::cout << "Superuser " << superUserName << " exists." << std::endl;
				}
				else {
					std::cout << "Superuser " << superUserName << " does not exist." << std::endl;
				}
				break;
			}
			case 2: {
				std::cout << "Enter the superuser name to create: ";
				std::cin >> superUserName;

				std::cout << "Enter password (leave blank for no password): ";
				std::cin >> password;

				if (manager.createSuperUser(superUserName, password)) {
					std::cout << "Superuser created successfully." << std::endl;
				}
				else {
					std::cerr << "Failed to create superuser." << std::endl;
				}
				break;
			}
			case 3: {
				manager.listSuperUsers();
				break;
			}
			case 4: {
				std::cout << "Exiting program." << std::endl;
				return; // exit
			}
			default: {
				std::cerr << "Invalid choice. Please enter a valid option." << std::endl;
				break;
			}
			}
		}
	}
} // namespace pgsqlSuperUser
