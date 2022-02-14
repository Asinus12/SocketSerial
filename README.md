# SocketSerial #

**Terminal app for sending arbitrary commands over local network**  
I have no space on my desk!  
Server side of the application is (intended to be) running on a Raspberry pi with access to internet
and connected to microcontrolles via RS232 (dongle).  

Client side can be run on any PC in the same subnet. (Windows machine needs testing)
You can send commands manually or put them into a text file named commands.txt with the following format:

    string:string:integer

No empty lines or characthers!!!

## Compilation ## 

- Server: clear && gcc -o server server.c && sudo ./server 3000 /dev/ttyUSB0
- Client: clear && gcc -o client client.c && ./client localhost 3000

Port numbers can be anything between 1 and 65535 (well known ports)

## Testing ##

For testing on a single machine, use above commands and have a USB-RS232 dongle plugged in with TX and RX connected.  
For testing on 2 machines, change the localhost to the IP address of the server.  

## Errors and Other ##
- You must run the server side with root privileges (klogctl)
- server_multi can serve multiple slaves, otherwise there is no difference from server (todo)
- If you get ERROR on binding: address already in use change the port number 

### Debugging ###
- lsof -i TCP:3000 ... shows open files
- valgrind -v ./client ... for debugging segmentation faults
- GDB "cheat sheat"  

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
            $ info line * 0x08000000 ... disassembly of memory location
            $ info mem 
            $ info threads
            $ display, undisplay <num>
            $ i r ... shows general purpose registers with CSPR 
            $ backtrace full
            $ kill run

-  http://www.linuxhowtos.org/C_C++/socket.htm
