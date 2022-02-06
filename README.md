# SocketSerial #

**Application for sending serial commands via internet socket**
Client sends commands to the server.
Server receives commands and sends them to serial communicaton port.

The goal is to have a server app runing on a local machine that is connected to embedded devices over RS232 USB dongle.
User can then sends commands (manually or from a file) to the server on the same subnet.


## Server ##
- compile with gcc -o server server.c
- run with ./server 51717


## Client ## 
- Compile with gcc -o client client.c 
- run with ./client localhost 51717 (if running on a single machine )
- run with ./clinet server_ip_address 51717 (if running on different machine on the same subnet)


## Errors and Other ##
- runable with port numbers from 2000 to 65535
- server_multi can serve multiple slaves, otherwise there is no difference from server 
- if you get ERROR on binding: address already in use change the port number 
- you can run server and connect to it with web browser with url localhost:51717 
- You have to have USB-RS232 usb plugged in (with RX and TX connected) for program to fully work 
-  http://www.linuxhowtos.org/C_C++/socket.htm
