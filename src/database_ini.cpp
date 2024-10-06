#include "database_ini.h"

#include <iostream>
#include "libpq-fe.h"

const char* createProductsTableSQL = R"(
    CREATE TABLE IF NOT EXISTS Products (
        product_id SERIAL PRIMARY KEY,
        name TEXT NOT NULL,
        price NUMERIC(10, 2) NOT NULL,
        stock INTEGER NOT NULL,
        category TEXT,
        is_deleted BOOLEAN DEFAULT FALSE
    );
)";

const char* createEmployeesTableSQL = R"(
    CREATE TABLE IF NOT EXISTS Employees (
        employee_id SERIAL PRIMARY KEY,
        name TEXT NOT NULL,
        position TEXT NOT NULL,
        hire_date DATE NOT NULL,
        contact_info TEXT,
        is_deleted BOOLEAN DEFAULT FALSE
    );
)";

const char* createOrdersTableSQL = R"(
    CREATE TABLE IF NOT EXISTS Orders (
        order_id SERIAL PRIMARY KEY,
        order_date DATE NOT NULL,
        employee_id INTEGER,
        customer_id INTEGER,
        total NUMERIC(10, 2) NOT NULL,
        status TEXT NOT NULL,
        is_deleted BOOLEAN DEFAULT FALSE,
        FOREIGN KEY (employee_id) REFERENCES Employees(employee_id),
        FOREIGN KEY (customer_id) REFERENCES Customers(customer_id)
    );
)";

const char* createOrderItemsTableSQL = R"(
    CREATE TABLE IF NOT EXISTS Order_Items (
        order_item_id SERIAL PRIMARY KEY,
        order_id INTEGER NOT NULL,
        product_id INTEGER NOT NULL,
        quantity INTEGER NOT NULL,
        price NUMERIC(10, 2) NOT NULL,
        is_deleted BOOLEAN DEFAULT FALSE,
        FOREIGN KEY (order_id) REFERENCES Orders(order_id),
        FOREIGN KEY (product_id) REFERENCES Products(product_id)
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

    const char* createTableSQL[] = {
        createEmployeesTableSQL,
        createOrdersTableSQL,
        createProductsTableSQL,
        createOrderItemsTableSQL
    };

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
    } else {
        std::cout << "Database already exists." << std::endl;
        return true;
    }
}

bool createUserAndDatabase(const std::string& inputDbName, const std::string& inputUserName,
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
    std::string conninfo = "dbname=" + (dbName.empty() ? "pet_store_db" : dbName) +
                           " user=" + (userName.empty() ? "pet_store_user" : userName) +
                           " password=" + (password.empty() ? "12345678" : password) +
                           " host=localhost port=5432";

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

