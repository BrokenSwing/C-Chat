# C-Chat ![CI CMake](https://github.com/BrokenSwing/C-Chat/workflows/CI%20CMake/badge.svg)

*A C-based chat system*

![](assets/logo-128x128.png)

## Table of content

* [Releases](#releases)
* [Building](#building-manually)
* [Running](#running)
* [Features](#features)

## Releases

Releases of the client and the server can be found in [releases tab](https://github.com/BrokenSwing/C-Chat/releases).

| Version | Multiple clients support (2+) | Parallel messages sending/receiving | File exchange support | Rooms                |
|:-------:|:-----------------------------:|:-----------------------------------:|:---------------------:|:--------------------:|
| v1.0.0  |   :x:                         |  :x:                                |   :x:                 | :x:                  |
| v2.0.0  |   :x:                         |  :white_check_mark:                 |   :x:                 | :x:                  |
| v3.0.0  |   :white_check_mark:          |  :white_check_mark:                 |   :x:                 | :x:                  |
| v4.0.0  |   :white_check_mark:          |  :white_check_mark:                 |   :white_check_mark:  | :x:                  |
| v5.0.0  |   :white_check_mark:          |  :white_check_mark:                 |   :white_check_mark:  | :white_check_mark:   |

## Building manually

* Use CMake
* Using gcc
    * Unix : `gcc -lpthread src/client/** src/common/** -o client` and `gcc -lpthread src/client/** src/common/** -o server`
    * Windows : `gcc src/client/** src/common/** -o client` and `gcc src/client/** src/common/** -o server`
    
## Running

Start server binary then client binary.
* Unix : `./server` then `./client`
* Windows : `server.exe` then `client.exe`

## Features

#### Commands

You can use commands on client, here are the available commands :
* `/nick <new_username>` : defines your new username
* `/file <send | receive>`
    * `/file send <filename>` : sends the file with the given name to other clients of the room
    * `/file receive <file_id>` : receives the file with the given id (file sent by a client)
* `/quit` : quits the server
* `/room <create | join | leave | list>`
    * `/room create <name> <description>` : creates a room with the given name and the given description
    * `/room join <name>` : joins the room with the given name
    * `/room leave` : leaves the room you're currently in
    * `/room list` : list all existing rooms