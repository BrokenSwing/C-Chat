# C-Chat ![CI CMake](https://github.com/BrokenSwing/C-Chat/workflows/CI%20CMake/badge.svg)

*A C-based chat system*

## Building

* Use CMake
* Use `gcc` :
    * Build client : `gcc client.c sockets.c -o client`
    * Build server : `gcc server.c sockets.c -o serveur`
    
## Running

Start server binary then client binary.
* Unix : `./server` then `./client`
* Windows : `server.exe` then `client.exe`