cmake_minimum_required(VERSION 3.0...3.5)

# the name of this CMake project and what language it uses
project(RentKeeper LANGUAGES CXX)

# C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(
        # 必要选项（建议保留）
        -Wall                             # 启用几乎所有常规警告
        -Wextra                           # 启用更多的附加警告
        -Werror                           # 将所有警告视为错误（开发过程中建议保留，生产环境可禁用）
        -pedantic-errors                  # 严格遵守标准 C++ 规范
        -Wuninitialized                   # 警告未初始化的变量
        -Wnull-dereference                # 捕获空指针解引用
        -Wnon-virtual-dtor                # 类有虚函数但缺少虚析构函数时警告
        -fstack-protector-strong          # 启用堆栈保护，防止栈溢出攻击

        # 可选项（根据需求启用）
        # -Wconversion                    # 警告隐式类型转换，可能会触发大量警告
        # -Wdouble-promotion              # 警告 float 被提升为 double 的情况，视情况启用
        -Woverloaded-virtual            # 警告派生类隐藏了基类的虚函数，适合多态场景
        -Wdeprecated-copy-dtor          # 警告不推荐的拷贝构造函数或析构函数
        # -Wold-style-cast                # 警告 C 风格的类型转换，建议使用 C++ 风格
        -Wzero-as-null-pointer-constant # 警告使用 0 作为空指针，建议使用 nullptr
        -Wsuggest-override              # 建议在虚函数重写时使用 override 关键字
        # -Wcast-align                    # 警告强制类型转换可能导致的内存对齐问题
        # -Wformat=2                      # 更严格的格式化字符串检查，主要用于 printf/scanf 等
)


# Enable sanitizers for debug builds
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-fsanitize=address,undefined")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "-fsanitize=address,undefined")
set(CMAKE_CXX_EXE_LINKER_FLAGS_DEBUG_INIT "-fsanitize=address,undefined")
set(CMAKE_CXX_EXE_LINKER_FLAGS_RELWITHDEBINFO_INIT "-fsanitize=address,undefined")

# ------------------------------------------------------------ #
# -------------- ADD YOUR NEW FILES BELOW THIS LINE ---------- #
# ------------------------------------------------------------ #

# Enable CTest for testing
enable_testing()

# 主程序
add_executable(main_exe src/main.cpp
        src/test.h
        src/test.cpp
        src/pgsql/database_ini.cpp
        src/pgsql/database_ini.h
        src/pgsql/database_drop.cpp
        src/pgsql/database_drop.h
        src/pgsql/pgsql_superuser.cpp
        src/pgsql/pgsql_superuser.h
        src/pgsql/pgsql_management.h
        src/pgsql/pgsql_management.cpp)

# 测试程序
add_executable(main_tests tests/main.test.cpp ../lib/catch_amalgamated.cpp
        src/test.h
        src/test.cpp)

# Register the tests
add_test(NAME main_test COMMAND main_tests)

# 设置编译选项，禁用未使用的变量警告（仅针对测试目标）
target_compile_options(main_tests PRIVATE -Wno-unused-but-set-variable)


if(APPLE)
    # macOS
#    find_package(SQLite3 REQUIRED)
    find_package(PostgreSQL REQUIRED)
    target_link_libraries(main_exe PRIVATE  PostgreSQL::PostgreSQL)

elseif(UNIX)
    # Linux
#    find_package(SQLite3 REQUIRED)
    find_package(PostgreSQL REQUIRED)
    target_link_libraries(main_exe PRIVATE  PostgreSQL::PostgreSQL)
endif()




