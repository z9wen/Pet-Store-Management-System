#ifndef DATABASE_INI_H
#define DATABASE_INI_H

#include <iostream>
#include <string>

bool initializeDatabase(const char* conninfo);
bool checkAndInitializeDatabase(const char* conninfo);
bool createUserAndDatabase(const std::string& inputDbName,
                           const std::string& inputUserName,
                           const std::string& inputPassword,
                           const std::string& superUserName = "postgres",
                           const std::string& superUserPassword = "");
void getUserInputAndCreateDatabase(const std::string& superUserName = "postgres",
                                   const std::string& superUserPassword = "");
bool checkDatabaseExists(const std::string& dbName,
                         const std::string& superUserName = "postgres",
                         const std::string& superUserPassword = "");

#endif // DATABASE_INI_H
