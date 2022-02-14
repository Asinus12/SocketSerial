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
#include <sys/klog.h> /* Read/clear kernel messages */



void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    

    // USB from CLI
    char* usbport = (char*) argv[2];
    printf("Connecting to: %s\n", usbport);

    // USB from dmesg 
    char kernel_buffer[100];
    char usb_name[13] = "/dev/ttyUSBX\0";
    int i = 8, j =5;

    // wrapper funciton of kernel funciton printk 
    // 2 - waits for new kernel messages, requires root privileges 
    // 3 - reads old kernel messages, does not require root privileges
    klogctl(3,kernel_buffer, sizeof(kernel_buffer));

    for(i; i>0;i--){
        usb_name[j++] = kernel_buffer[strlen(kernel_buffer)-i];
    }
    usb_name[j] = '\0';
    printf("Detected:      %s",usb_name);

    if(!strcmp(usbport,usb_name))
        printf("Ports matching!\n");


    












    // variables for internet socket  
    int sockfd, newsockfd, portno;
    socklen_t clilen;
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


    printf("Waiting for clients .. \n");

    // listen an accept an IPv4 socket
    listen(sockfd,5);

    clilen = sizeof(cli_addr);

    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
                
    if (newsockfd < 0) 
        error("ERROR on accept");
    

    /******************* IPv4 socket connection established *************************/


    char mybuffer[256];
    
    //  receive num of commands
    bzero(mybuffer,256);

    myn = read(newsockfd,mybuffer,255);
    if (myn < 0) error("ERROR reading from socket");

    printf("%s commands will be received ..\n",mybuffer);

    const unsigned short cmd_count = atoi(mybuffer);
    unsigned short cmdc = cmd_count;
    char cmd_array[cmdc][20];
    char cmdval_array[cmdc][20];
    char cmddly_array[cmdc][20];

    while(cmdc--){

        // receive command 
        bzero(mybuffer,256);
        myn = read(newsockfd,mybuffer,255);
        if (myn < 0)
            error("ERROR reading from socket");
            strcpy(cmd_array[cmdc], mybuffer);
            printf("Cmd %d: %s ",cmdc, cmd_array[cmdc]);

        // receive command value 
        bzero(mybuffer,256);
        myn = read(newsockfd,mybuffer,255);
        if (myn < 0)
            error("ERROR reading from socket");
            strcpy(cmdval_array[cmdc], mybuffer);
            printf("%s ",cmdval_array[cmdc]);


        // receive command delay 
        bzero(mybuffer,256);
        myn = read(newsockfd,mybuffer,255);
        if (myn < 0)
            error("ERROR reading from socket");
            strcpy(cmddly_array[cmdc], mybuffer);
            printf("%s",cmddly_array[cmdc]);

    }
    printf("\n");
    close(newsockfd);
    close(sockfd);


     /****************** Establishing serial connection  *********************************/

    
    printf("\n");
    printf("Opening serial port on %s\n", usbport);

    // File Descriptor for the port
    int fd = open(usbport, O_RDWR | O_NOCTTY | O_NDELAY);
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


    /************************* Serial connection established  *********************************/


    // variables for serial communication 
    char rxbuffer[256];
    int bytes, n;
    
    cmdc = cmd_count; 
    while (cmdc--)
    {
        // WRITE PART, write to the serial file descriptor
        printf("TX: %s \n", cmd_array[cmdc]);
        n = write(fd, cmd_array[cmdc], 7);
        if (n < 0) {
            fputs("write() failed!\n", stderr);
        }

        usleep(atoi(cmddly_array[cmdc])*1000);
        
        //READ PART, read from serial file descriptor 
        bytes = read(fd, rxbuffer, sizeof(rxbuffer));
        printf("RX:  %s", rxbuffer);
        printf("\n");

        usleep(atoi(cmddly_array[cmdc])*1000);

    }
    
    close(fd);
    printf("Closed serial port\n");




    // return from main 
    return 0; 
}
