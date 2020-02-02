cmake_minimum_required(VERSION 3.13.4)
project(wps)
set (CMAKE_CXX_STANDARD 17)

add_subdirectory(third_party/leveldb)

# Wips.cc and leveldb
add_executable(wips Wips.cc)
add_executable(dbscript db_script.cc includes/Lexer.cc includes/XMLParser.cc)

find_package(sdbus-c++ REQUIRED)
target_link_libraries(wips PRIVATE SDBusCpp::sdbus-c++)
target_link_libraries(wips PUBLIC leveldb)
target_link_libraries(dbscript PUBLIC leveldb)
