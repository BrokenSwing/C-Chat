# cmake_minimum_required(VERSION <specify CMake version here>)
project(Projet_FAR C)

set(CMAKE_C_STANDARD 99)

add_executable(Client src/client/client.c src/common/sockets.c)
add_executable(Server src/server/server.c src/common/sockets.c)