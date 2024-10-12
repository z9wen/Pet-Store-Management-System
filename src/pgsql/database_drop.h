#ifndef DATABASE_DROP_H
#define DATABASE_DROP_H

#include "libpq-fe.h"
#include <iostream>
#include <string>

bool dropProductsTable(PGconn* conn);
bool dropEmployeesTable(PGconn* conn);
bool dropOrdersTable(PGconn* conn);
bool dropOrderItemsTable(PGconn* conn);
bool dropCustomersTable(PGconn* conn);
bool dropSuppliersTable(PGconn* conn);
bool dropInventoryActionsTable(PGconn* conn);
bool dropAllTables(const char* conninfo);
bool dropDatabase(const std::string& dbName,
                  const std::string& superUserName = "postgres",
                  const std::string& superUserPassword = "");

#endif // DATABASE_DROP_H
