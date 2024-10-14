#ifndef DATABASE_DROP_H
#define DATABASE_DROP_H

#include <libpq-fe.h>
#include <iostream>

namespace pgsqlDropDatabase {
	class DatabaseDropManager {
	 public:
		// Constructor: Initializes the connection string with database, user, and password
		DatabaseDropManager(const std::string& dbName, const std::string& userName, const std::string& Password);

		// Destructor: Close the connection
		~DatabaseDropManager();

		// Connect to the database
		bool connect();

		// Drop all tables
		bool dropAllTables();

		// Drop a specific table
		bool dropSpecificTable(const std::string& tableName);

		// Drop a specific database
		bool
		dropDatabase(const std::string& dbName, const std::string& superUserName, const std::string& superUserPassword);

	 private:
		PGconn* conn_;
		std::string conninfo_;

		// Drop individual tables
		bool dropProductsTable();
		bool dropEmployeesTable();
		bool dropOrdersTable();
		bool dropOrderItemsTable();
		bool dropCustomersTable();
		bool dropSuppliersTable();
		bool dropInventoryActionsTable();

		// Execute the drop statement for a table
		bool executeDrop(const char* dropSQL, const std::string& tableName);
	};
} // namespace pgsqlDropDatabase

#endif // DATABASE_DROP_H
