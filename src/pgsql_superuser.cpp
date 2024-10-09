#include "pgsql_superuser.h"

#include "libpq-fe.h"
#include <iostream>
#include <string>

// 检查系统中是否存在超级用户
bool checkAnySuperUserExists(PGconn* conn) {
    const char* query = "SELECT rolname FROM pg_roles WHERE rolsuper = true;";
    PGresult* res = PQexec(conn, query);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Failed to check for superusers: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        return false;
    }

    int rows = PQntuples(res);
    PQclear(res);

    return rows > 0;  // 返回 true 表示至少存在一个超级用户
}

// 检查指定用户是否为超级用户
bool isUserSuperUser(PGconn* conn, const std::string& superUserName) {
    std::string query = "SELECT rolname FROM pg_roles WHERE rolname = '" + superUserName + "' AND rolsuper = true;";
    PGresult* res = PQexec(conn, query.c_str());

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Failed to check if user is superuser: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        return false;
    }

    int rows = PQntuples(res);
    PQclear(res);

    return rows > 0;  // 如果返回行数大于 0，表示该用户是超级用户
}

// 创建新的超级用户
bool createSuperUser(PGconn* conn, const std::string& superUserName, const std::string& password) {
    // 构建创建超级用户的 SQL 语句
    std::string createUserSQL = "CREATE USER " + superUserName + " WITH SUPERUSER CREATEDB CREATEROLE LOGIN";
    if (!password.empty()) {
        createUserSQL += " PASSWORD '" + password + "';";
    } else {
        createUserSQL += ";"; // 如果密码为空，SQL 语句中不包含密码部分
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

// 管理超级用户：检查是否存在并根据需要创建
bool manageSuperUser(const std::string& superUserName, const std::string& password, const std::string& conninfo) {
    // 连接到数据库
    PGconn* conn = PQconnectdb(conninfo.c_str());
    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Connection to database failed: " << PQerrorMessage(conn) << std::endl;
        PQfinish(conn);
        return false;
    }

    // 检查指定超级用户是否存在
    if (isUserSuperUser(conn, superUserName)) {
        std::cout << "Superuser " << superUserName << " already exists." << std::endl;
    } else {
        // 如果不存在，则创建一个新的超级用户
        std::cout << "Creating new superuser: " << superUserName << std::endl;
        if (!createSuperUser(conn, superUserName, password)) {
            std::cerr << "Failed to create superuser." << std::endl;
            PQfinish(conn);
            return false;
        }
        std::cout << "Superuser created successfully." << std::endl;
    }

    // 关闭数据库连接
    PQfinish(conn);
    return true;
}

