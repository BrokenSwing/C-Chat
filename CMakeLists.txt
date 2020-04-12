# cmake_minimum_required(VERSION <specify CMake version here>)
project(Projet_FAR C)

set(CMAKE_C_STANDARD 99)

add_executable(Client client.c sockets.c)
add_executable(Server server.c sockets.c)