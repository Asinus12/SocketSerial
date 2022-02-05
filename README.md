#SocketSerial
Example of a stream socket in internet domain with serial communication on server side





### Server ###
- compile with gcc -o server server.c
- run with ./server 51717


### Client ### 
- Compile with gcc -o client client.c 
- run with ./client localhost 51717 (if running on a single machine )


### Errors and Other ###
- runable with port numbers from 2000 to 65535

- if you get ERROR on binding: address already in use change the port number 

- you can run server and connect to it with web browser with url localhost:51717 and 
  it prints out some werid text worthy of exploring 

-  http://www.linuxhowtos.org/C_C++/socket.htm
