cmake_minimum_required(VERSION 3.0)

project(onderGrup)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")

set(MARIADB_CONNECTOR_C_DIR "C:/Program Files (x86)/MariaDB/MariaDB Connector C")

include_directories(${MARIADB_CONNECTOR_C_DIR}/include)

link_directories(${MARIADB_CONNECTOR_C_DIR}/lib)

add_executable(onderGrup main.c)

target_link_libraries(onderGrup libmariadb)
