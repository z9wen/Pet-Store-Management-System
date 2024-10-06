#include <iostream>

#include "test.h"
#include "database_ini.h"
#include "database_drop.h"

auto main() -> int {
    std::string dbName = "pet_store_db";
    // 检查数据库是否存在
    if (!checkDatabaseExists(dbName)) {
        std::cout << "Database does not exist. Initializing..." << std::endl;
        getUserInputAndCreateDatabase();  // 数据库不存在时创建
    } else {
        std::cout << "Database already exists." << std::endl;
    }

    return 0;
}
