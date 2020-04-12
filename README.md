# C-Chat ![CI CMake](https://github.com/BrokenSwing/C-Chat/workflows/CI%20CMake/badge.svg)

*A C-based chat system*

## Building

* Use CMake
* Use `gcc` :
    * Build client : `gcc src/client.c src/sockets.c -o client`
    * Build server : `gcc src/server.c src/sockets.c -o server`
    
## Running

Start server binary then client binary.
* Unix : `./server` then `./client`
* Windows : `server.exe` then `client.exe`