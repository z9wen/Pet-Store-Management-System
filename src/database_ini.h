#ifndef DATABASE_INI_H
#define DATABASE_INI_H

#include <iostream>
#include <string>

bool initializeDatabase(const char* dbName);
bool checkDatabaseExists(const char* dbName);
bool checkAndInitializeDatabase(const char* dbName);
bool createUserAndDatabase(const std::string& inputDbName,
                           const std::string& inputUserName,
                           const std::string& inputPassword);
void getUserInputAndCreateDatabase(const std::string& superUserName = "postgres",
                                   const std::string& superUserPassword = "");
bool checkDatabaseExists(const std::string& dbName);

#endif // DATABASE_INI_H
