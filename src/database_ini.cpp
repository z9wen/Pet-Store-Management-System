#include "database_ini.h"

#include "libpq-fe.h"
#include <iostream>

const char* createProductsTableSQL = R"(
    CREATE TABLE IF NOT EXISTS Products (
        product_id SERIAL PRIMARY KEY,  -- Auto-incrementing unique identifier for each product
        name TEXT NOT NULL,  -- Name of the product
        price NUMERIC(10, 2) NOT NULL,  -- Price of the product with two decimal precision
        stock INTEGER NOT NULL,  -- Current stock quantity of the product
        category TEXT,  -- Product category or classification
        is_deleted BOOLEAN DEFAULT FALSE  -- Soft delete flag, marks whether the product is logically deleted
    );
)";

const char* createEmployeesTableSQL = R"(
    CREATE TABLE IF NOT EXISTS Employees (
        employee_id SERIAL PRIMARY KEY,  -- Auto-incrementing unique identifier for each employee
        name TEXT NOT NULL,  -- Name of the employee
        position TEXT NOT NULL,  -- Job position of the employee
        hire_date DATE NOT NULL,  -- Date when the employee was hired
        contact_info TEXT,  -- Contact details of the employee
        is_deleted BOOLEAN DEFAULT FALSE  -- Soft delete flag, marks whether the employee is logically deleted
    );
)";

const char* createOrdersTableSQL = R"(
    CREATE TABLE IF NOT EXISTS Orders (
        order_id SERIAL PRIMARY KEY,  -- Auto-incrementing unique identifier for each order
        order_date DATE NOT NULL,  -- Date when the order was placed
        employee_id INTEGER,  -- Reference to the employee who handled the order (foreign key to Employees table)
        customer_id INTEGER,  -- Reference to the customer who placed the order (foreign key to Customers table)
        total NUMERIC(10, 2) NOT NULL,  -- Total amount of the order, with two decimal precision
        status TEXT NOT NULL,  -- Status of the order (e.g., pending, completed)
        is_deleted BOOLEAN DEFAULT FALSE,  -- Soft delete flag, marks whether the order is logically deleted
        FOREIGN KEY (employee_id) REFERENCES Employees(employee_id),  -- Foreign key to Employees table
        FOREIGN KEY (customer_id) REFERENCES Customers(customer_id)  -- Foreign key to Customers table
    );
)";

const char* createOrderItemsTableSQL = R"(
    CREATE TABLE IF NOT EXISTS Order_Items (
        order_item_id SERIAL PRIMARY KEY,  -- Auto-incrementing unique identifier for each order item
        order_id INTEGER NOT NULL,  -- Reference to the order (foreign key to Orders table)
        product_id INTEGER NOT NULL,  -- Reference to the product in the order (foreign key to Products table)
        quantity INTEGER NOT NULL,  -- Quantity of the product ordered
        price NUMERIC(10, 2) NOT NULL,  -- Price of the product at the time of order, with two decimal precision
        is_deleted BOOLEAN DEFAULT FALSE,  -- Soft delete flag, marks whether the order item is logically deleted
        FOREIGN KEY (order_id) REFERENCES Orders(order_id),  -- Foreign key to Orders table
        FOREIGN KEY (product_id) REFERENCES Products(product_id)  -- Foreign key to Products table
    );
)";

const char* createCustomersTableSQL = R"(
    CREATE TABLE IF NOT EXISTS Customers (
        customer_id SERIAL PRIMARY KEY,  -- Auto-incrementing unique identifier for each customer
        name TEXT NOT NULL,  -- Name of the customer
        phone_number TEXT NOT NULL,  -- Customer's phone number
        email TEXT NOT NULL,  -- Customer's email address
        address TEXT NOT NULL,  -- Customer's address
        is_deleted BOOLEAN DEFAULT FALSE  -- Soft delete flag, marks whether the customer is logically deleted
    );
)";

const char* createSuppliersTableSQL = R"(
    CREATE TABLE IF NOT EXISTS Suppliers (
        supplier_id SERIAL PRIMARY KEY,  -- Auto-incrementing unique identifier for each supplier
        name TEXT NOT NULL,  -- Name of the supplier
        contact_info TEXT,  -- Supplier's contact details
        product_id INTEGER,  -- Reference to the product supplied by this supplier (foreign key to Products table)
        is_deleted BOOLEAN DEFAULT FALSE,  -- Soft delete flag, marks whether the supplier is logically deleted
        FOREIGN KEY (product_id) REFERENCES Products(product_id)  -- Foreign key to Products table
    );
)";

const char* createInventoryActionsTableSQL = R"(
    CREATE TABLE IF NOT EXISTS Inventory_Actions (
        action_id SERIAL PRIMARY KEY,  -- Auto-incrementing unique identifier for each inventory action
        product_id INTEGER NOT NULL,  -- Reference to the product for the inventory action (foreign key to Products table)
        action_type TEXT NOT NULL,  -- Type of inventory action (e.g., inbound, outbound)
        quantity INTEGER NOT NULL,  -- Quantity of the inventory action
        action_date DATE NOT NULL,  -- Date when the inventory action took place
        is_deleted BOOLEAN DEFAULT FALSE,  -- Soft delete flag, marks whether the inventory action is logically deleted
        FOREIGN KEY (product_id) REFERENCES Products(product_id)  -- Foreign key to Products table
    );
)";

// Initialize the database connection and create tables
bool initializeDatabase(const char* conninfo) {
	PGconn* conn = PQconnectdb(conninfo);
	if (PQstatus(conn) != CONNECTION_OK) {
		std::cerr << "Connection to database failed: " << PQerrorMessage(conn) << std::endl;
		PQfinish(conn);
		return false;
	}

	const char* createTableSQL[] = {createEmployeesTableSQL,
	                                createOrdersTableSQL,
	                                createProductsTableSQL,
	                                createOrderItemsTableSQL,
	                                createCustomersTableSQL,
	                                createSuppliersTableSQL,
	                                createInventoryActionsTableSQL};

	// Execute SQL statements
	for (const char* sql : createTableSQL) {
		PGresult* res = PQexec(conn, sql);
		if (PQresultStatus(res) != PGRES_COMMAND_OK) {
			std::cerr << "Failed to create table: " << PQerrorMessage(conn) << std::endl;
			PQclear(res);
			PQfinish(conn);
			return false;
		}
		PQclear(res);
	}

	PQfinish(conn);
	std::cout << "Database initialized successfully." << std::endl;
	return true;
}

// Check if database connection is valid
bool checkDatabaseExists(const char* conninfo) {
	PGconn* conn = PQconnectdb(conninfo);
	if (PQstatus(conn) != CONNECTION_OK) {
		std::cerr << "Connection to database failed: " << PQerrorMessage(conn) << std::endl;
		PQfinish(conn);
		return false;
	}
	PQfinish(conn);
	return true;
}

bool checkAndInitializeDatabase(const char* conninfo) {
	if (!checkDatabaseExists(conninfo)) {
		std::cout << "Database does not exist, initializing..." << std::endl;
		return initializeDatabase(conninfo);
	}
	else {
		std::cout << "Database already exists." << std::endl;
		return true;
	}
}

bool createUserAndDatabase(const std::string& inputDbName,
                           const std::string& inputUserName,
                           const std::string& inputPassword) {
	// default
	std::string dbName = inputDbName.empty() ? "pet_store_db" : inputDbName;
	std::string userName = inputUserName.empty() ? "pet_store_user" : inputUserName;
	std::string password = inputPassword.empty() ? "12345678" : inputPassword;

	// create user
	std::string createUserSQL = "CREATE USER " + userName + " WITH ENCRYPTED PASSWORD '" + password + "';";
	std::string createDatabaseSQL = "CREATE DATABSE " + dbName + ";";
	std::string grantPrivilegesSQL = "GRANT ALL PRIVILEGES ON DATABASE " + dbName + " TO " + userName + ";";

	// superuser create pgsql
	// const char* conninfo = "dbname=postgres user=postgres password=superuser_password host=localhost port=5432";
	const char* conninfo = "dbname=postgres user=postgres host=localhost port=5432";
	PGconn* conn = PQconnectdb(conninfo);

	if (PQstatus(conn) != CONNECTION_OK) {
		std::cerr << "Connection to database failed: " << PQerrorMessage(conn) << std::endl;
		PQfinish(conn);
		return false;
	}

	// Execute create user SQL
	PGresult* res = PQexec(conn, createUserSQL.c_str());
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		std::cerr << "Failed to create user: " << PQerrorMessage(conn) << std::endl;
		PQclear(res);
		PQfinish(conn);
		return false;
	}
	PQclear(res);

	// Execute SQL to create database
	res = PQexec(conn, createDatabaseSQL.c_str());
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		std::cerr << "Failed to create database: " << PQerrorMessage(conn) << std::endl;
		PQclear(res);
		PQfinish(conn);
		return false;
	}
	PQclear(res);

	// Execute the SQL statement to grant permissions
	res = PQexec(conn, grantPrivilegesSQL.c_str());
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		std::cerr << "Failed to grant privileges: " << PQerrorMessage(conn) << std::endl;
		PQclear(res);
		PQfinish(conn);
		return false;
	}
	PQclear(res);

	// close connection
	PQfinish(conn);
	std::cout << "User and database created successfully." << std::endl;
	return true;
}

void getUserInputAndCreateDatabase() {
	std::string dbName, userName, password;

	// users input
	std::cout << "Enter database name (leave blank for default 'pet_store_db'): ";
	std::getline(std::cin, dbName);

	std::cout << "Enter user name (leave blank for default 'pet_store_user'): ";
	std::getline(std::cin, userName);

	std::cout << "Enter password (leave blank for default '12345678'): ";
	std::getline(std::cin, password);

	// create user and database
	if (!createUserAndDatabase(dbName, userName, password)) {
		std::cerr << "Failed to create user and database." << std::endl;
		return;
	}

	// Prepare database connection information for initializing the table
	std::string conninfo = "dbname=" + (dbName.empty() ? "pet_store_db" : dbName)
	                       + " user=" + (userName.empty() ? "pet_store_user" : userName)
	                       + " password=" + (password.empty() ? "12345678" : password) + " host=localhost port=5432";

	// initialize table
	if (!initializeDatabase(conninfo.c_str())) {
		std::cerr << "Failed to initialize tables." << std::endl;
	}
}

bool checkDatabaseExists(const std::string& dbName) {
	// std::string conninfo = "dbname=" + dbName +
	//     " user=postgres password=superuser_password host=localhost port=5432";
	std::string conninfo = "dbname=" + dbName + " user=postgres host=localhost port=5432";
	PGconn* conn = PQconnectdb(conninfo.c_str());

	// check connection
	if (PQstatus(conn) != CONNECTION_OK) {
		std::cerr << "Database does not exist: " << PQerrorMessage(conn) << std::endl;
		PQfinish(conn);
		return false;
	}

	PQfinish(conn);
	return true;
}
