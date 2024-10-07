#include <iostream>
#include <string>
#include <vector>

#include "database_drop.h"
#include "database_ini.h"
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

	std::string dbName = "pet_store_db";
	// 检查数据库是否存在
	if (!checkDatabaseExists(dbName)) {
		std::cout << "Database does not exist. Initializing..." << std::endl;
		getUserInputAndCreateDatabase(); // 数据库不存在时创建
	}
	else {
		std::cout << "Database already exists." << std::endl;
	}

	return 0;
}
