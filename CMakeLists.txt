cmake_minimum_required(VERSION 3.10)
project(EchoApp C)

set(CMAKE_C_STANDARD 11)

add_executable(EchoApp src/main.c src/FrameSock.c headers/FrameInet.h src/FrameServer.c)