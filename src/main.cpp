#include <iostream>
#include <string>
#include <vector>

#include "database_drop.h"
#include "database_ini.h"
#include "pgsql_management.h"
#include "pgsql_superuser.h"
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

	pgsqlSuperUser::sqlSuperUsersManagementMenu();

	return 0;
}
