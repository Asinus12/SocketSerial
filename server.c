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



    // Check CLI arguments 
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

    // create a IPv4 socket 
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    error("ERROR opening socket");

    bzero((char *) &serv_addr, sizeof(serv_addr));

    // retrieve portnumber from arguments 
    portno = atoi(argv[1]);

    // set parameters for IPv4 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    // bind a socket to an address 
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR on binding");

    // listen an accept an IPv4 socket
    listen(sockfd,5);

    clilen = sizeof(cli_addr);

    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
                
    if (newsockfd < 0) 
        error("ERROR on accept");
    

    /*** IPv4 connection established ***/
    //  receive num of commands
    bzero(mybuffer,256);

    myn = read(newsockfd,mybuffer,255);
    if (myn < 0) error("ERROR reading from socket");

    printf("SERVER: Receivng %s commands ..\n",mybuffer);
    const unsigned short cmd_count = atoi(mybuffer);
    unsigned short cmdc = cmd_count;

    // char array for commands
    char cmd_array[cmdc][20];

    // while loop for parsing text file 
    while(cmdc--){
        bzero(mybuffer,256);
        myn = read(newsockfd,mybuffer,255);
        if (myn < 0)
            error("ERROR reading from socket");
            strcpy(cmd_array[cmd_count], mybuffer);
            printf("SERVER: Received message: %s\n",cmd_array[cmd_count]);

    }


    close(newsockfd);
    close(sockfd);
    printf("Commands parsed, opening serial port on /dev/ttyUSB0\n");


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



    // variables for serial communication 
    static char xbuffer[7] = "lala56";
    int bytes, n;


    cmdc = cmd_count; 
    while (cmdc--)
    {
        // WRITE PART, write to the serial file descriptor
        n = write(fd, xbuffer, sizeof(xbuffer));
        sleep(1);
    
        // check if ok
        if (n < 0) {
            fputs("write() failed!\n", stderr);
        }


        //READ PART, read from serial file descriptor 
        bytes = read(fd, &xbuffer, sizeof(xbuffer));
        
        printf("number of bytes read is %d\n", bytes );
        printf("SERVER: %s\n", xbuffer);
    }
    




 






    close(fd);
    printf("Closed serial port\n");




    // return from main 
    return 0; 
}
