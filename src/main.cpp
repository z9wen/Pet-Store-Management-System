#include <iostream>
#include <string>
#include <vector>

#include "pgsql/database_drop.h"
#include "pgsql/database_ini.h"
#include "pgsql/pgsql_superuser.h"
#include "test.h"

void printAscii() {
	std::vector<std::string> storeManagementAscii = {
	    "=============================================================================================================="
	    "================================",
	    "            _      __ _                                                                             _      __ "
	    "          _                 ",
	    " _ __   ___| |_   / _\\ |_ ___  _ __ ___     /\\/\\   __ _ _ __   __ _  __ _  ___ _ __ ___   ___ _ __ | |_   "
	    "/ _\\_   _ ___| |_ ___ _ __ ___  ",
	    "| '_ \\ / _ \\ __|  \\ \\| __/ _ \\| '__/ _ \\   /    \\ / _` | '_ \\ / _` |/ _` |/ _ \\ '_ ` _ \\ / _ \\ '_ "
	    "\\| __|  \\ \\| | | / __| __/ _ \\ '_ ` _ \\ ",
	    "| |_) |  __/ |_   _\\ \\ || (_) | | |  __/  / /\\/\\ \\ (_| | | | | (_| | (_| |  __/ | | | | |  __/ | | | |_  "
	    " _\\ \\ |_| \\__ \\ ||  __/ | | | | |",
	    "| .__/ \\___|\\__|  \\__/\\__\\___/|_|  \\___|  \\/    \\/\\__,_|_| |_|\\__,_|\\__, |\\___|_| |_| |_|\\___|_| "
	    "|_|\\__|  \\__/\\__, |___/\\__\\___|_| |_| |_|",
	    "|_|                                                                 |___/                                     "
	    "|___/                       ",
	    "=============================================================================================================="
	    "================================"};

	for (const auto& line : storeManagementAscii) {
		std::cout << line << std::endl;
	}
}

auto main() -> int {
	printAscii();

	int choice;
	while (true) {
		std::cout << "\n=== Main Menu ===" << std::endl;
		std::cout << "1. Super User Management" << std::endl;
		std::cout << "2. PostgreSQL Initialization" << std::endl;
		std::cout << "3. Exit" << std::endl;
		std::cout << "Enter your choice: ";
		std::cin >> choice;

		switch (choice) {
			case 1: {
				// 调用超级用户管理菜单
				pgsqlSuperUser::sqlSuperUsersManagementMenu();
				break;
			}
			case 2: {
				// 调用PostgreSQL初始化菜单
				pgsqlInitialization::pgsqlInitializationManagementMenu();
				break;
			}
			case 3: {
				std::cout << "Exiting..." << std::endl;
				return 0;  // 退出程序
			}
			default: {
				std::cout << "Invalid choice. Please try again." << std::endl;
				break;
			}
		}
	}

	return 0;
}
