#include "database_ini.h"

namespace pgsqlInitialization {
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

	const char* createSuppliersTableSQL = R"(
	    CREATE TABLE IF NOT EXISTS Suppliers (
	        supplier_id SERIAL PRIMARY KEY,  -- Auto-incrementing unique identifier for each supplier
	        name TEXT NOT NULL,  -- Name of the supplier
	        contact_info TEXT,  -- Supplier's contact details
	        product_id INTEGER,  -- Reference to the product supplied by this supplier (foreign key to Products table)
	        is_deleted BOOLEAN DEFAULT FALSE  -- Soft delete flag, marks whether the supplier is logically deleted
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

	// Constructor that sets up connection information for the superuser
	DatabaseInitializer::DatabaseInitializer(const std::string& superUserName, const std::string& superUserPassword) {
		superUserConnInfo_ = "dbname=postgres user=" + superUserName + " host=localhost port=5432";
		if (!superUserPassword.empty()) {
			superUserConnInfo_ += " password=" + superUserPassword;
		}
		conn_ = nullptr; // Initialize the connection pointer to null
	}

	// Destructor to close the PostgreSQL connection
	DatabaseInitializer::~DatabaseInitializer() {
		if (conn_) {
			PQfinish(conn_);
		}
	}

	// Method to establish a connection using the superuser credentials
	bool DatabaseInitializer::checkDatabaseExists(const std::string& dbName) {
		std::string conninfo = "dbname=" + dbName + " user=postgres host=localhost port=5432";
		conn_ = PQconnectdb(conninfo.c_str());

		if (PQstatus(conn_) != CONNECTION_OK) {
			std::cerr << "Database does not exist: " << PQerrorMessage(conn_) << std::endl;
			PQfinish(conn_);
			return false;
		}

		PQfinish(conn_);
		return true;
	}

	bool DatabaseInitializer::createUserAndDatabase(const std::string& dbName,
	                                                const std::string& userName,
	                                                const std::string& password) {
		conn_ = PQconnectdb(superUserConnInfo_.c_str());
		if (PQstatus(conn_) != CONNECTION_OK) {
			std::cerr << "Connection to database failed: " << PQerrorMessage(conn_) << std::endl;
			PQfinish(conn_);
			return false;
		}

		// SQL statements to create the user and database
		std::string createUserSQL = "CREATE USER " + userName + " WITH ENCRYPTED PASSWORD '" + password + "';";
		std::string createDatabaseSQL = "CREATE DATABASE " + dbName + ";";
		std::string grantPrivilegesSQL = "GRANT ALL PRIVILEGES ON DATABASE " + dbName + " TO " + userName + ";";

		// Execute user creation
		PGresult* res = PQexec(conn_, createUserSQL.c_str());
		if (PQresultStatus(res) != PGRES_COMMAND_OK) {
			std::cerr << "Failed to create user: " << PQerrorMessage(conn_) << std::endl;
			PQclear(res);
			PQfinish(conn_);
			return false;
		}
		PQclear(res);

		// Execute database creation
		res = PQexec(conn_, createDatabaseSQL.c_str());
		if (PQresultStatus(res) != PGRES_COMMAND_OK) {
			std::cerr << "Failed to create database: " << PQerrorMessage(conn_) << std::endl;
			PQclear(res);
			PQfinish(conn_);
			return false;
		}
		PQclear(res);

		// Grant privileges
		res = PQexec(conn_, grantPrivilegesSQL.c_str());
		if (PQresultStatus(res) != PGRES_COMMAND_OK) {
			std::cerr << "Failed to grant privileges: " << PQerrorMessage(conn_) << std::endl;
			PQclear(res);
			PQfinish(conn_);
			return false;
		}
		PQclear(res);

		PQfinish(conn_);
		return true;
	}

	// Method to initialize tables in the database
	bool DatabaseInitializer::initializeTables(const std::string& dbName,
	                                           const std::string& userName,
	                                           const std::string& password) {
		std::string conninfo = "dbname=" + dbName + " user=" + userName + " host=localhost port=5432";
		if (!password.empty()) {
			conninfo += " password=" + password;
		}

		conn_ = PQconnectdb(conninfo.c_str());
		if (PQstatus(conn_) != CONNECTION_OK) {
			std::cerr << "Connection to database failed: " << PQerrorMessage(conn_) << std::endl;
			PQfinish(conn_);
			return false;
		}

		const char* createTableSQL[] = {createCustomersTableSQL,
		                                createProductsTableSQL,
		                                createEmployeesTableSQL,
		                                createOrdersTableSQL,
		                                createOrderItemsTableSQL,
		                                createSuppliersTableSQL,
		                                createInventoryActionsTableSQL};

		for (const char* sql : createTableSQL) {
			PGresult* res = PQexec(conn_, sql);
			if (PQresultStatus(res) != PGRES_COMMAND_OK) {
				std::cerr << "Failed to create table: " << PQerrorMessage(conn_) << std::endl;
				PQclear(res);
				PQfinish(conn_);
				return false;
			}
			PQclear(res);
		}

		PQfinish(conn_);
		std::cout << "Tables initialized successfully." << std::endl;
		return true;
	}

	void pgsqlInitializationMenuShow() {
		std::cout << "\n==== PostgreSQL Database Management ====" << std::endl;
		std::cout << "1. Check if Database Exists" << std::endl;
		std::cout << "2. Create User and Database" << std::endl;
		std::cout << "3. Initialize Tables" << std::endl;
		std::cout << "4. Exit" << std::endl;
		std::cout << "========================================" << std::endl;
		std::cout << "Enter your choice: ";
	}

	void pgsqlInitializationManagementMenu() {
		int choice;
		std::string dbName, userName, password;
		std::string superUserName, superUserPassword;

		// Get superuser connection information
		std::cout << "Enter superuser name (default is 'postgres'): ";
		std::getline(std::cin, superUserName);
		if (superUserName.empty()) {
			superUserName = "postgres";
		}

		std::cout << "Enter superuser password (leave blank for no password): ";
		std::getline(std::cin, superUserPassword);

		// Instantiate the DatabaseInitializer class
		pgsqlInitialization::DatabaseInitializer dbInitializer(superUserName, superUserPassword);

		while (true) {
			// show menu
			pgsqlInitializationMenuShow();
			std::cin >> choice;

			if (std::cin.fail()) {
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cerr << "Invalid input. Please enter a number." << std::endl;
				continue;
			}

			switch (choice) {
			case 1: { // check database exists
				std::cout << "Enter database name to check: ";
				std::cin >> dbName;

				if (dbInitializer.checkDatabaseExists(dbName)) {
					std::cout << "Database " << dbName << " exists." << std::endl;
				}
				else {
					std::cout << "Database " << dbName << "does not exist." << std::endl;
				}
				break;
			}
			case 2: { // create user and database
				std::cout << "Enter new database name: ";
				std::cin >> dbName;

				std::cout << "Enter new user name: ";
				std::cin >> userName;

				std::cout << "Enter password for new user (leave blank for default '1234567890'): ";
				std::cin >> password;
				if (password.empty()) {
					password = "1234567890";
				}

				if (dbInitializer.createUserAndDatabase(dbName, userName, password)) {
					std::cout << "User and database created successfully." << std::endl;
				}
				else {
					std::cout << "Failed to create user and database." << std::endl;
				}
				break;
			}
			case 3: { // Initialize the tables
				std::cout << "Enter database name to initialize: ";
				std::cin >> dbName;

				std::cout << "Enter user name to initialize tables: ";
				std::cin >> userName;

				std::cout << "Enter password for user (leave blank if none): ";
				std::cin >> password;

				if (dbInitializer.initializeTables(dbName, userName, password)) {
					std::cout << "Tables initialized successfully." << std::endl;
				}
				else {
					std::cout << "Failed to initialize tables." << std::endl;
				}
				break;
			}
			case 4: { // exit
				std::cout << "Exiting program..." << std::endl;
				return;
			}
			default: std::cerr << "Invalid choice. Please enter a valid option." << std::endl; break;
			}
		}
	}
} // namespace pgsqlInitialization