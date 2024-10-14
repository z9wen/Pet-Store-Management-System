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

} // namespace pgsqlDropDatabase
