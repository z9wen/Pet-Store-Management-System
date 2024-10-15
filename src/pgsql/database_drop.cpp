#include "database_drop.h"
#include <libpq-fe.h>
#include <iostream>

namespace pgsqlDropDatabase {
	// Constructor: Initializes the connection string with database, user, and password
	DatabaseDropManager::DatabaseDropManager(const std::string& dbName,
	                                         const std::string& userName,
	                                         const std::string& password) {
		conninfo_ = "dbname=" + dbName + " user=" + userName + " host=localhost port=5432";
		if (!password.empty()) {
			conninfo_ += " password=" + password;
		}
		conn_ = nullptr;
	}

	// Destructor: Close the connection
	DatabaseDropManager::~DatabaseDropManager() {
		if (conn_) {
			PQfinish(conn_);
		}
	}

	// Connect to the database
	bool DatabaseDropManager::connect() {
		conn_ = PQconnectdb(conninfo_.c_str());
		if (PQstatus(conn_) != CONNECTION_OK) {
			std::cerr << "Connection to database failed: " << PQerrorMessage(conn_) << std::endl;
			PQfinish(conn_);
			conn_ = nullptr;
			return false;
		}
		return true;
	}

	// Drop a specific table by name
	bool DatabaseDropManager::dropSpecificTable(const std::string& tableName) {
		std::string dropTableSQL = "DROP TABLE IF EXISTS " + tableName + ";";
		return executeDrop(dropTableSQL.c_str(), tableName);
	}

	// Drop all tables
	bool DatabaseDropManager::dropAllTables() {
		if (!dropProductsTable() || !dropEmployeesTable() || !dropOrdersTable() || !dropOrderItemsTable()
		    || !dropCustomersTable() || !dropSuppliersTable() || !dropInventoryActionsTable())
		{
			return false;
		}
		std::cout << "All tables dropped successfully." << std::endl;
		return true;
	}

	// Drop a specific database
	bool DatabaseDropManager::dropDatabase(const std::string& dbName,
	                                       const std::string& superUserName,
	                                       const std::string& superUserPassword) {
		std::string conninfo = "dbname=postgres user=" + superUserName + " host=localhost port=5432";
		if (!superUserPassword.empty()) {
			conninfo += " password=" + superUserPassword;
		}

		PGconn* superuser_conn = PQconnectdb(conninfo.c_str());
		if (PQstatus(superuser_conn) != CONNECTION_OK) {
			std::cerr << "Connection to postgres database failed: " << PQerrorMessage(superuser_conn) << std::endl;
			PQfinish(superuser_conn);
			return false;
		}

		std::string dropDatabaseSQL = "DROP DATABASE IF EXISTS " + dbName + ";";
		PGresult* res = PQexec(superuser_conn, dropDatabaseSQL.c_str());

		if (PQresultStatus(res) != PGRES_COMMAND_OK) {
			std::cerr << "Failed to drop database: " << PQerrorMessage(superuser_conn) << std::endl;
			PQclear(res);
			PQfinish(superuser_conn);
			return false;
		}

		PQclear(res);
		PQfinish(superuser_conn);
		std::cout << "Database '" << dbName << "' dropped successfully." << std::endl;
		return true;
	}

	// Private method to execute the drop table SQL commands
	bool DatabaseDropManager::executeDrop(const char* dropSQL, const std::string& tableName) {
		PGresult* res = PQexec(conn_, dropSQL);
		if (PQresultStatus(res) != PGRES_COMMAND_OK) {
			std::cerr << "Failed to drop " << tableName << " table: " << PQerrorMessage(conn_) << std::endl;
			PQclear(res);
			return false;
		}
		PQclear(res);
		std::cout << tableName << " table dropped successfully." << std::endl;
		return true;
	}

	PGconn* DatabaseDropManager::getConnection() const {
		return conn_;
	}

	// Drop individual tables
	bool DatabaseDropManager::dropProductsTable() {
		return executeDrop("DROP TABLE IF EXISTS Products;", "Products");
	}

	bool DatabaseDropManager::dropEmployeesTable() {
		return executeDrop("DROP TABLE IF EXISTS Employees;", "Employees");
	}

	bool DatabaseDropManager::dropOrdersTable() {
		return executeDrop("DROP TABLE IF EXISTS Orders;", "Orders");
	}

	bool DatabaseDropManager::dropOrderItemsTable() {
		return executeDrop("DROP TABLE IF EXISTS Order_Items;", "Order Items");
	}

	bool DatabaseDropManager::dropCustomersTable() {
		return executeDrop("DROP TABLE IF EXISTS Customers;", "Customers");
	}

	bool DatabaseDropManager::dropSuppliersTable() {
		return executeDrop("DROP TABLE IF EXISTS Suppliers;", "Suppliers");
	}

	bool DatabaseDropManager::dropInventoryActionsTable() {
		return executeDrop("DROP TABLE IF EXISTS Inventory_Actions;", "Inventory Actions");
	}

	void pgsqlDropMenuShow() {
		std::cout << "\n==== PostgreSQL Database Drop Debug Menu ====" << std::endl;
		std::cout << "1. Drop specific table" << std::endl;
		std::cout << "2. Drop all tables" << std::endl;
		std::cout << "3. Drop database" << std::endl;
		std::cout << "4. Exit" << std::endl;
		std::cout << "=============================================" << std::endl;
		std::cout << "Enter your choice: ";
	}

	void pgsqlDropManagementMenu() {
		std::string dbName, userName, password, superUserName, superUserPassword, tableName;
		int choice;

		std::cout << "Enter database name: ";
		std::getline(std::cin, dbName);
		std::cout << "Enter username: ";
		std::getline(std::cin, userName);
		std::cout << "Enter password (leave blank for no password): ";
		std::getline(std::cin, password);

		// Instantiate the DatabaseDropManager
		pgsqlDropDatabase::DatabaseDropManager dbDropManager(dbName, userName, password);

		// error connect to the database
		if (!dbDropManager.connect()) {
			std::cerr << "Failed to connect to the database." << std::endl;
			return;
		}

		while (true) {
			pgsqlDropMenuShow();
			std::cin >> choice;
			std::cin.ignore(); // ignore newline after entering the number

			switch (choice) {
			case 1: { // Drop a specific table
				// List all tables before asking for input
				PGresult* res = PQexec(dbDropManager.getConnection(),
				                       "SELECT tablename FROM pg_tables WHERE schemaname = 'public';");

				if (PQresultStatus(res) != PGRES_TUPLES_OK) {
					std::cerr << "Failed to retrieve tables: " << PQerrorMessage(dbDropManager.getConnection())
					          << std::endl;
					PQclear(res);
					break;
				}

				int rows = PQntuples(res);
				if (rows == 0) {
					std::cout << "No tables found in the current database." << std::endl;
					PQclear(res);
					break;
				}

				std::cout << "\nAvailable tables:" << std::endl;
				for (int i = 0; i < rows; i++) {
					std::cout << "- " << PQgetvalue(res, i, 0) << std::endl;
				}
				PQclear(res);

				// Drop a specific table
				std::cout << "\nEnter the table name to drop: ";
				std::getline(std::cin, tableName);

				if (dbDropManager.dropSpecificTable(tableName)) {
					std::cout << "Table " << tableName << " dropped successfully." << std::endl;
				}
				else {
					std::cerr << "Failed to drop table " << tableName << "." << std::endl;
				}
				break;
			}
			case 2: {
				// Drop all tables
				if (dbDropManager.dropAllTables()) {
					std::cout << "All tables dropped successfully." << std::endl;
				}
				else {
					std::cerr << "Failed to drop all tables." << std::endl;
				}
				break;
			}
			case 3: {
				// Drop the database
				std::cout << "Enter superuser name (default is 'postgres'): ";
				std::getline(std::cin, superUserName);
				if (superUserName.empty()) {
					superUserName = "postgres"; // default to postgres
				}

				std::cout << "Enter superuser password (leave blank for no password): ";
				std::getline(std::cin, superUserPassword);

				if (dbDropManager.dropDatabase(dbName, superUserName, superUserPassword)) {
					std::cout << "Database '" << dbName << "' dropped successfully." << std::endl;
				}
				else {
					std::cerr << "Failed to drop database '" << dbName << "'." << std::endl;
				}
				return; // exit after dropping the database
			}
			case 4: {
				// Exit
				std::cout << "Exiting..." << std::endl;
				return;
			}
			default: {
				std::cerr << "Invalid choice. Please enter a valid option." << std::endl;
				break;
			}
			}
		}
	}

} // namespace pgsqlDropDatabase
