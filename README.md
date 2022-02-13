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
- CAUTION: commands.txt file must not have any empty lines, gives seg error
- You may have to run the program with root privileges (klogctl)
- Runable with port numbers from 2000 to 65535
- server_multi can serve multiple slaves, otherwise there is no difference from server 
- If you get ERROR on binding: address already in use change the port number 
- you can run server and connect to it with web browser with url localhost:51717 
- You have to have USB-RS232 usb plugged in (with RX and TX connected) for program to fully work 
### Debugging ###
- lsof -i TCP:51717 ... shows open files
- valgrind -v ./client ... for debugging segmentation faults
- debug with 

            $ cc -g client.c
            $ gdb a.out
            $ gdb --args a.out arg1 arg2 arg3 .. run with CLI arguments
            $ layout next .. rotates perspectives
            $ set print elements 0 ... if gdb is truncating printf 
            $ b main ... sets breakpoint at main
            $ b 73 ... sets breakpoint at line 73
            $ run
            $ continue
            $ next .. dont dive 
            $ step .. dive into 
            $ finish ... continue until current fucntion returns 
            $ info line * 0x08000000 ... disassembly of qpfn_vectors
            $ info mem 
            $ info threads
            $ display, undisplay <num>, enable/disable display <num>
            $ i r ... show all registers r0-r15
            $ backtrace full
            $ kill run

-  http://www.linuxhowtos.org/C_C++/socket.htm
