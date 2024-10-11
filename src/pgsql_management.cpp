#include "pgsql_superuser.h"
#include "libpq-fe.h"
#include <iostream>
#include <limits>

void sqlManagementMenuShow() {
    std::cout << "==== PostgreSQL Superuser Management ====" << std::endl;
    std::cout << "1. Check if Superuser Exists" << std::endl;
    std::cout << "2. Create Superuser" << std::endl;
    std::cout << "3. List All Superusers" << std::endl;
    std::cout << "4. Exit" << std::endl;
    std::cout << "=========================================" << std::endl;
    std::cout << "Enter your choice: ";
}

std::string getSuperUserConnectionInfo(std::string& superUserName, std::string& superUserPassword) {
    std::cout << "Enter superuser name (default is 'postgres'): ";
    std::getline(std::cin, superUserName);
    if (superUserName.empty()) {
        superUserName = "postgres";  // default pgsql user postgres
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

    // Get the superuser connection information
    conninfo = getSuperUserConnectionInfo(connectionSuperUser, connectionPassword);

    while (true) {
        sqlManagementMenuShow();  // show menu
        std::cin >> choice;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
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
                    continue;  // back to menu
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
                    continue;  // back to menu
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
                PGconn* conn = PQconnectdb(conninfo.c_str());
                checkAnySuperUserExists(conn);
                break;
            }
            case 4: {
                std::cout << "Exiting program." << std::endl;
                return;  // exit
            }
            default: {
                std::cerr << "Invalid choice. Please enter a valid option." << std::endl;
                break;
            }
        }
    }
}

