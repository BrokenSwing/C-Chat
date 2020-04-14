# C-Chat ![CI CMake](https://github.com/BrokenSwing/C-Chat/workflows/CI%20CMake/badge.svg)

*A C-based chat system*

![](assets/logo-512x512.png)

## Building

* Use CMake
* Use `gcc` :
    * Build client : `gcc src/client/client.c src/common/sockets.c -o client`
    * Build server : `gcc src/server/server.c src/common/sockets.c -o server`
    
## Running

Start server binary then client binary.
* Unix : `./server` then `./client`
* Windows : `server.exe` then `client.exe`