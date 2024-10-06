#ifndef DATABASE_DROP_H
#define DATABASE_DROP_H

#include "libpq-fe.h"

bool dropProductsTable(PGconn* conn);
bool dropEmployeesTable(PGconn* conn);
bool dropOrdersTable(PGconn* conn);
bool dropOrderItemsTable(PGconn* conn);
bool dropAllTables(const char* conninfo);

#endif //DATABASE_DROP_H
