#include "database_ini.h"

#include "sqlite3.h"
#include <vector>

const char* createProductsTableSQL = R"(
    CREATE TABLE IF NOT EXISTS Products (
        product_id INTEGER PRIMARY KEY AUTOINCREMENT,
        name TEXT NOT NULL,
        price REAL NOT NULL,
        stock INTEGER NOT NULL,
        category TEXT
    );
)";

const char* createEmployeesTableSQL = R"(
    CREATE TABLE IF NOT EXISTS Employees (
        employee_id INTEGER PRIMARY KEY AUTOINCREMENT,
        name TEXT NOT NULL,
        position TEXT NOT NULL,
        hire_date DATE NOT NULL,
        contact_info TEXT
    );
)";

const char* createOrdersTableSQL = R"(
    CREATE TABLE IF NOT EXISTS Orders (
        order_id INTEGER PRIMARY KEY AUTOINCREMENT,
        order_date DATE NOT NULL,
        employee_id INTEGER,
        total REAL NOT NULL,
        status TEXT NOT NULL,
        FOREIGN KEY (employee_id) REFERENCES Employees(employee_id)
    );
)";

const char* createOrderItemsTableSQL = R"(
    CREATE TABLE IF NOT EXISTS Order_Items (
        order_item_id INTEGER PRIMARY KEY AUTOINCREMENT,
        order_id INTEGER NOT NULL,
        product_id INTEGER NOT NULL,
        quantity INTEGER NOT NULL,
        price REAL NOT NULL,
        FOREIGN KEY (order_id) REFERENCES Orders(order_id),
        FOREIGN KEY (product_id) REFERENCES Products(product_id)
    );
)";

void initializeDatabase() {


}