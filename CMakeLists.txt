cmake_minimum_required(VERSION 3.10)
project(EchoApp C)

set(CMAKE_C_STANDARD 11)
set(SOURCE src/FrameSock.c headers/FrameInet.h headers/DefInfo.h  src/multiplex.c headers/multiplex.h)
set(TCP_SOURCE ${SOURCE} src/FrameTcp.c headers/FrameTcp.h)
set(UDP_SOURCE ${SOURCE} src/FrameUdp.c headers/FrameUdp.h)
set(BROADCAST_SOURCE ${UDP_SOURCE} headers/Broadcast.h src/broadcast.c)

add_executable(Client_tcp exec/client_tcp.c ${TCP_SOURCE})
add_executable(Server_tcp exec/server_tcp.c ${TCP_SOURCE})
add_executable(Serv_select_tcp exec/serv_select_tcp.c ${TCP_SOURCE})
add_executable(Serv_poll_tcp exec/serv_poll_tcp.c ${TCP_SOURCE})
add_executable(Serv_epoll_tcp exec/serv_epoll_tcp.c ${TCP_SOURCE})

add_executable(Client_udp exec/client_udp.c ${UDP_SOURCE})
add_executable(Server_udp exec/server_udp.c ${UDP_SOURCE})

add_executable(Broadcast_serv exec/bcast_serv.c ${BROADCAST_SOURCE})
add_executable(Broadcast_client exec/bcast_client.c ${BROADCAST_SOURCE})