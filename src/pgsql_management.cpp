#include "pgsql_superuser.h"
#include "libpq-fe.h"
#include <iostream>
#include <limits>

void sqlManagementMenuShow() {
    std::cout << "==== PostgreSQL Superuser Management ====" << std::endl;
    std::cout << "1. Check if Superuser Exists" << std::endl;
    std::cout << "2. Create Superuser" << std::endl;
    std::cout << "3. Exit" << std::endl;
    std::cout << "=========================================" << std::endl;
    std::cout << "Enter your choice: ";
}

std::string getSuperUserConnectionInfo(std::string& superUserName, std::string& superUserPassword) {
    std::cout << "Enter superuser name (default is 'postgres'): ";
    std::getline(std::cin, superUserName);
    if (superUserName.empty()) {
        superUserName = "postgres";  // 默认使用 "postgres" 超级用户
    }

    std::cout << "Enter superuser password (leave blank for no password): ";
    std::getline(std::cin, superUserPassword);

    std::string conninfo = "dbname=postgres user=" + superUserName;
    if (!superUserPassword.empty()) {
        conninfo += " password=" + superUserPassword;
    }
    conninfo += " host=localhost port=5432";

    return conninfo;
}

void sqlManagementMenu() {
    int choice;
    std::string superUserName;
    std::string password;

    std::string conninfo;
    std::string connectionSuperUser;
    std::string connectionPassword;

    // 获取超级用户的连接信息
    conninfo = getSuperUserConnectionInfo(connectionSuperUser, connectionPassword);

    while (true) {
        sqlManagementMenuShow();  // 显示菜单选项
        std::cin >> choice;

        if (std::cin.fail()) {  // 检查输入是否有效
            std::cin.clear();   // 清除错误标志
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 忽略无效输入
            std::cerr << "Invalid input. Please enter a number." << std::endl;
            continue;
        }

        switch (choice) {
            case 1: {
                std::cout << "Enter the superuser name to check: ";
                std::cin >> superUserName;

                PGconn* conn = PQconnectdb(conninfo.c_str());
                if (PQstatus(conn) != CONNECTION_OK) {
                    std::cerr << "Connection to database failed: " << PQerrorMessage(conn) << std::endl;
                    PQfinish(conn);
                    continue;  // 返回到菜单
                }

                if (isUserSuperUser(conn, superUserName)) {
                    std::cout << "Superuser " << superUserName << " exists." << std::endl;
                } else {
                    std::cout << "Superuser " << superUserName << " does not exist." << std::endl;
                }

                PQfinish(conn);
                break;
            }
            case 2: {
                std::cout << "Enter the superuser name to create: ";
                std::cin >> superUserName;

                std::cout << "Enter password (leave blank for no password): ";
                std::cin >> password;

                PGconn* conn = PQconnectdb(conninfo.c_str());
                if (PQstatus(conn) != CONNECTION_OK) {
                    std::cerr << "Connection to database failed: " << PQerrorMessage(conn) << std::endl;
                    PQfinish(conn);
                    continue;  // 返回到菜单
                }

                if (createSuperUser(conn, superUserName, password)) {
                    std::cout << "Superuser created successfully." << std::endl;
                } else {
                    std::cerr << "Failed to create superuser." << std::endl;
                }

                PQfinish(conn);
                break;
            }
            case 3: {
                std::cout << "Exiting program." << std::endl;
                return;  // 退出函数和循环
            }
            default: {
                std::cerr << "Invalid choice. Please enter a valid option." << std::endl;
                break;
            }
        }
    }
}

