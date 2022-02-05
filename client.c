/* Client */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];

    // Checks if app is run with proper arguments 
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    // retrieves port number 
    portno = atoi(argv[2]);

    // creates a socket (domain, type, protocol)
    // DOMAIN:
    //   AF_INET - IPv4 internet protocol 
    //   AF_NETLINK - Kernel user interface device 
    // TYPE: 
    //   SOCK_STREAM - Sequenced reliable two way connection based byte stream 

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    // retrieves server address (localhost) from arguments
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    // erases the n bytes of memory starting at memory location of first argument
    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;

    // copies n bytes from src to dest 
    bcopy((char *)server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);

    
    serv_addr.sin_port = htons(portno);
     
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");

    // User enters the message 
    printf("Please enter the message: ");

    // erases buffer
    bzero(buffer,256);

    // fill buffer with standard input 
    fgets(buffer,255,stdin);

    // write() ... writes to file descriptor
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
         error("ERROR writing to socket");

    // clear buffer 
    bzero(buffer,256);

    // reads from file descriptor 
    n = read(sockfd,buffer,255);
    if (n < 0) 
         error("ERROR reading from socket");


    printf("Received message :");

    // i got your message 
    printf("%s\n",buffer);


    close(sockfd);
    return 0;
}
