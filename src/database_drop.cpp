#include "database_drop.h"
#include <iostream>
#include <libpq-fe.h>  // PostgreSQL C API

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

// drop all tables
bool dropAllTables(const char* conninfo) {
    // 连接到 PostgreSQL 数据库
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

    // close db
    PQfinish(conn);
    std::cout << "All tables dropped successfully." << std::endl;
    return true;
}