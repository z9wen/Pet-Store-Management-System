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

	std::string choice_str;
	int choice;

	while (true) {
		std::cout << "\n=== Main Menu ===" << std::endl;
		std::cout << "1. Super User Management" << std::endl;
		std::cout << "2. PostgreSQL Initialization" << std::endl;
		std::cout << "3. Exit" << std::endl;
		std::cout << "Enter your choice: ";
		std::getline(std::cin, choice_str);

		// string to int
		try {
			choice = std::stoi(choice_str);
		} catch (const std::invalid_argument& e) {
			std::cout << "Invalid input. Please enter a valid number." << std::endl;
			continue;
		}

		switch (choice) {
		case 1: {
			// Invoking the Superuser Menu
			pgsqlSuperUser::sqlSuperUsersManagementMenu();
			break;
		}
		case 2: {
			// Invoking the pgsqlInitialization Menu
			pgsqlInitialization::pgsqlInitializationManagementMenu();
			break;
		}
		case 3: {
			std::cout << "Exiting..." << std::endl;
			return 0;
		}
		default: {
			std::cout << "Invalid choice. Please try again." << std::endl;
			break;
		}
		}
	}

	return 0;
}