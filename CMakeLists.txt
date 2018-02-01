cmake_minimum_required(VERSION 3.10)
project(EchoApp C)

set(CMAKE_C_STANDARD 11)
set(SOURCE src/FrameSock.c headers/FrameInet.h headers/DefInfo.h headers/FrameTcp.h)
set(TCP_SOURCE ${SOURCE} src/FrameTcp.c)

add_executable(Server_tcp src/server_tcp.c ${TCP_SOURCE})
add_executable(Client_tcp src/client_tcp.c ${TCP_SOURCE})