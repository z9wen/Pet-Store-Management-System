#include "database_drop.h"
#include <libpq-fe.h>
#include <iostream>

// drop products table
bool dropProductsTable(PGconn* conn) {
	const char* dropProductsSQL = "DROP TABLE IF EXISTS Products;";
	PGresult* res = PQexec(conn, dropProductsSQL);

	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		std::cerr << "Failed to drop Products table: " << PQerrorMessage(conn) << std::endl;
		PQclear(res);
		return false;
	}
	PQclear(res);
	return true;
}

// drop employees table
bool dropEmployeesTable(PGconn* conn) {
	const char* dropEmployeesTable = "DROP TABLE IF EXISTS Employees;";
	PGresult* res = PQexec(conn, dropEmployeesTable);

	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		std::cerr << "Failed to drop Employees table: " << PQerrorMessage(conn) << std::endl;
		PQclear(res);
		return false;
	}
	PQclear(res);
	return true;
}

// drop orders table
bool dropOrdersTable(PGconn* conn) {
	const char* dropOrdersSQL = "DROP TABLE IF EXISTS Orders;";
	PGresult* res = PQexec(conn, dropOrdersSQL);

	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		std::cerr << "Failed to drop Orders table: " << PQerrorMessage(conn) << std::endl;
		PQclear(res);
		return false;
	}
	PQclear(res);
	return true;
}

// drop orderItems table
bool dropOrderItemsTable(PGconn* conn) {
	const char* dropOrderItemsSQL = "DROP TABLE IF EXISTS Order_Items;";
	PGresult* res = PQexec(conn, dropOrderItemsSQL);

	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		std::cerr << "Failed to drop Order_Items table: " << PQerrorMessage(conn) << std::endl;
		PQclear(res);
		return false;
	}
	PQclear(res);
	return true;
}

// drop customers table
bool dropCustomersTable(PGconn* conn) {
	const char* dropCustomersSQL = "DROP TABLE IF EXISTS Customers;";
	PGresult* res = PQexec(conn, dropCustomersSQL);

	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		std::cerr << "Failed to drop Customers table: " << PQerrorMessage(conn) << std::endl;
		PQclear(res);
		return false;
	}
	PQclear(res);
	return true;
}

// drop suppliers table
bool dropSuppliersTable(PGconn* conn) {
	const char* dropSuppliersSQL = "DROP TABLE IF EXISTS Suppliers;";
	PGresult* res = PQexec(conn, dropSuppliersSQL);

	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		std::cerr << "Failed to drop Suppliers table: " << PQerrorMessage(conn) << std::endl;
		PQclear(res);
		return false;
	}
	PQclear(res);
	return true;
}

// drop inventory actions table
bool dropInventoryActionsTable(PGconn* conn) {
	const char* dropInventoryActionsSQL = "DROP TABLE IF EXISTS Inventory_Actions;";
	PGresult* res = PQexec(conn, dropInventoryActionsSQL);

	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		std::cerr << "Failed to drop Inventory_Actions table: " << PQerrorMessage(conn) << std::endl;
		PQclear(res);
		return false;
	}
	PQclear(res);
	return true;
}

// drop all tables
bool dropAllTables(const char* conninfo) {
	PGconn* conn = PQconnectdb(conninfo);
	if (PQstatus(conn) != CONNECTION_OK) {
		std::cerr << "Connection to database failed: " << PQerrorMessage(conn) << std::endl;
		PQfinish(conn);
		return false;
	}

	if (!dropProductsTable(conn)) {
		PQfinish(conn);
		return false;
	}
	if (!dropEmployeesTable(conn)) {
		PQfinish(conn);
		return false;
	}
	if (!dropOrdersTable(conn)) {
		PQfinish(conn);
		return false;
	}
	if (!dropOrderItemsTable(conn)) {
		PQfinish(conn);
		return false;
	}
	if (!dropCustomersTable(conn)) {
		PQfinish(conn);
		return false;
	}
	if (!dropSuppliersTable(conn)) {
		PQfinish(conn);
		return false;
	}
	if (!dropInventoryActionsTable(conn)) {
		PQfinish(conn);
		return false;
	}

	PQfinish(conn);
	std::cout << "All tables dropped successfully." << std::endl;
	return true;
}

bool dropDatabase(const std::string& dbName, const std::string& superUserName, const std::string& superUserPassword) {
	std::string conninfo = "dbname=postgres user=" + superUserName
	                       + (superUserPassword.empty() ? "" : " password=" + superUserPassword)
	                       + " host=localhost port=5432";
	PGconn* conn = PQconnectdb(conninfo.c_str());
	if (PQstatus(conn) != CONNECTION_OK) {
		std::cerr << "Connection to postgres database failed: " << PQerrorMessage(conn) << std::endl;
		PQfinish(conn);
		return false;
	}

	// Prepare SQL to drop the database
	std::string dropDatabaseSQL = "DROP DATABASE IF EXISTS " + dbName + ";";

	PGresult* res = PQexec(conn, dropDatabaseSQL.c_str());
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		std::cerr << "Failed to drop database: " << PQerrorMessage(conn) << std::endl;
		PQclear(res);
		PQfinish(conn);
		return false;
	}

	PQclear(res);
	PQfinish(conn);
	std::cout << "Database '" << dbName << " ' dropped successfully." << std::endl;
	return true;
}
