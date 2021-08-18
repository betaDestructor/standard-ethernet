# standard-ethernet
This is a C/C++ simulation project which illustrates the framing of standard ethernet protocol. It creates server and client processes on the same machine and through IPC, it sends the data from client to server in a simplex communication.

# Required softwares:
1. OS: Linux distro - 64bit.
2. Compilers: gcc and g++.

# Execution steps:
1. Open 2 terminals.
2. In the first terminal, run the following command:
  a. g++ -o server FrameEncode.cpp main.cpp
  b. ./server
3. In the second terminal, run the following command:
  a. g++ -o client FrameDecode.cpp main.cpp
  b. ./client
4. Perform the operation as per the program prompts.
