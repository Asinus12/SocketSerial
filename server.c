/* Server */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */


// sockfd and newsockfd are file descriptors, i.e. array subscripts into the file descriptor table.
// Each running process has a file descriptor table which
// contains pointers to all open i/o streams.  When a
// process starts, three entries are created in the first
// three cells of the table. stdin in cell one, stdout in cell two and stderr in cell 3 
// Whenever a file or other i/o stream is opened, a new entry is created in this table,
// usually in the first available empty slot.
// The socket system call returns an entry into this
// table; i.e.  a small integer.  This value is used for
// other calls which use this socket.  The accept system
// call returns another entry into this table.  The value
// returned by accept is used for reading and writing to
// that connection.



void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    
    // variables for internet socket  
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char mybuffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int myn;
    // variables for serial communication 
    static char xbuffer[7] = "lala56";
    int bytes, n;


    /****************** TCP socket *********************/
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

    // bind system call binds a socket to an address 
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
    error("ERROR on binding");

    // listen(file descriptor that refers to a socket of type SOCK_STREAM or SOCK_SEQPACKET,
    //        max length to which pending request queue may grow)
    listen(sockfd,5);

    clilen = sizeof(cli_addr);

    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
                
    if (newsockfd < 0) 
        error("ERROR on accept");
    

    // clear buffer for message 1 
    bzero(mybuffer,256);

    // read (system call) from file descriptor 
    myn = read(newsockfd,mybuffer,255);
    if (myn < 0) error("ERROR reading from socket");

    printf("SERVER: Received message: %s\n",mybuffer);

    close(newsockfd);
    close(sockfd);

    printf("Sockets closed! \n");
     

    /****************** Serial communication socket *************************/
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

    // Serial communication parameters 
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

    // set attributes NOW ( other options are FLUSH and DRAIN)
    tcsetattr(fd, TCSANOW, &options);

    // serial port opened 
    printf("Opened serial port on /dev/ttyUSB0\n");


  

    // write to the serial file descriptor
    n = write(fd, xbuffer, sizeof(xbuffer));

    // check if ok
    if (n < 0) {
        fputs("write() failed!\n", stderr);
    }

    // read from serial file descriptor 
    bytes = read(fd, &xbuffer, sizeof(xbuffer));
    
    printf("number of bytes read is %d\n", bytes );
    printf("%s\n", xbuffer);

    close(fd);
    printf("Closed serial port\n");




    // return from main 
    return 0; 
}
