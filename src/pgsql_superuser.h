#ifndef PGSQL_SUPERUSER_H
#define PGSQL_SUPERUSER_H

#include "libpq-fe.h"
#include <iostream>
#include <string>

bool checkSuperUserExits(PGconn* conn, const std::string& superUserName);
bool createSuperUser(PGconn* conn, const std::string& superUserName, const std::string& password);
bool manageSuperUser(const std::string& superUserName, const std::string& password);

#endif // PGSQL_SUPERUSER_H
