# cmake_minimum_required(VERSION <specify CMake version here>)
project(C-Chat C)

set(CMAKE_C_STANDARD 99)

add_executable(Client src/client/client.c src/common/sockets.c src/client/ui.c)
add_executable(Server src/server/server.c src/common/sockets.c)