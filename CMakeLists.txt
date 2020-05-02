cmake_minimum_required(VERSION 2.6)
project(C-Chat C)

set(CMAKE_C_STANDARD 99)
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads)

add_executable(Client
        src/client/client.c          src/client/client.h
        src/client/ui.c              src/client/ui.h
        src/client/console-color.h
        src/client/commands.h

        src/common/constants.h
        src/common/interop.h
        src/common/sockets.c         src/common/sockets.h
        src/common/threads.c         src/common/threads.h
        src/common/synchronization.c src/common/synchronization.h
        src/common/files.c           src/common/files.h
        src/common/packets.c         src/common/packets.h
)

add_executable(Server
        src/server/server.c          src/server/server.h

        src/common/constants.h
        src/common/interop.h
        src/common/sockets.c         src/common/sockets.h
        src/common/threads.c         src/common/threads.h
        src/common/synchronization.c src/common/synchronization.c
        src/common/files.c           src/common/files.h
        src/common/packets.c         src/common/packets.h
)

target_link_libraries(Client ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(Server ${CMAKE_THREAD_LIBS_INIT})