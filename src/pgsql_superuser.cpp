#include "pgsql_superuser.h"

#include <iostream>
#include "libpq-fe.h"

//  Check if there is a superuser
bool checkSuperUserExits(PGconn* conn, const std::string& superUserName) {
    std::string query = "SELECT rolname FROM pg_roles WHERE rolname = '"
                        + superUserName + "' AND rolesuper = true;";
    PGresult* res = PQexec(conn, query.c_str());

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Failed to check superiser: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        return false;
    }

    int rows = PQntuples(res);
    PQclear(res);

    return rows > 0; // If the number of rows returned is greater than 0, it means the user exists
}

bool createSuperUser(PGconn* conn, const std::string& superUserName, const std::string& password) {
    // If the password is an empty string, no password is set
    std::string createUserSQL = "CREATE USER " + superUserName + " WITH SUPERUSER CREATEDB CREATEROLE LGOIN";
    if (!password.empty()) {
        createUserSQL += " PASSWORD '" + password + "';";
    }else {
        createUserSQL += ";"; // When the password is empty, the SQL statement does not contain the password part
    }

    PGresult* res = PQexec(conn, createUserSQL.c_str());

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "Failed to create superuser: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        return false;
    }

    PQclear(res);
    return true;
}
