/* Server */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

//from serial port example 
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */




void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    
     int sockfd, newsockfd, portno;
     socklen_t clilen;
     char mybuffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int myn;

     // Check arguments 
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }

     // creates socket 
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");

     // fills location of first argument with zeros of size 
     bzero((char *) &serv_addr, sizeof(serv_addr));

     //retrieve portnumber from arguments 
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);

     // bind to socket 
     if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR on binding");

     // listen(file descriptor that refers to a socket of type SOCK_STREAM or SOCK_SEQPACKET,
     //        max length to which pending request queue may grow)
     listen(sockfd,5);

     clilen = sizeof(cli_addr);

     newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
                 
     if (newsockfd < 0) 
          error("ERROR on accept");
    
     //clear buffer 
     bzero(mybuffer,256);

     // read system call from file descriptor 
     myn = read(newsockfd,mybuffer,255);
     if (myn < 0) error("ERROR reading from socket");

    
     printf("Here is the message: %s\n",mybuffer);
     myn = write(newsockfd,"I got your message",18);
     if (myn < 0) error("ERROR writing to socket");
     close(newsockfd);
     close(sockfd);
     


    /* Serial communication part */

    printf("Opening serial port on /dev/ttyUSB0\n");
    
    // File Descriptor for the port
    int fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1){
        perror("Unable to open /dev/ttyUSB0");
    }
    
    // normal blocking mode, doesnt work with last argument FNDELAY
    fcntl(fd, F_SETFL, 0); 

    // get attributes from file descriptor and write them to options
    struct termios options;
    tcgetattr(fd, &options);

    // Setting serial communication parameters 
    cfsetispeed( &options, B9600 ); // Set input baud rate
    cfsetospeed( &options, B9600 ); // Set output baud rate 
    options.c_cflag |= ( CLOCAL | CREAD );
    options.c_cflag &= ~CSIZE; // Mask the character size bits 
    options.c_cflag |= CS8;    // Select 8 data bits character size 
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB; // Stop bit
    options.c_cflag &= ~CSIZE; // Character size 
    options.c_cflag |= CS8; 
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Disable HW flow control,RAW input,~CNEW_RTSCTS not supported
    options.c_iflag &= ~(IXON | IXOFF | IXANY); // Disable Software Flow control
    options.c_oflag &= ~OPOST; // RAW output

    // Set attributes NOW ( other options are FLUSH and DRAIN)
    tcsetattr(fd, TCSANOW, &options);


    printf("sending to port\n");
    //write to the port
    int n;
    
    n = write(fd, "1234", 4);

    if (n < 0) {
        fputs("write() of 1 bytes failed!\n", stderr);
    }

	printf("message sent\n");
    // Read from port
    char buffer[10];
    int bytes;
    bytes = read(fd, &buffer, sizeof(buffer));
    printf("number of bytes read is %d\n", bytes);
    printf("%s\n", buffer);

    
    close(fd);
    // end of serial comm project 
     return 0; 
}
