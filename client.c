/* Client side */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 


typedef struct node{
    char* cmd;
    char* value; 
    char* delay; 
    struct node* next;
}node_t;

void error(const char *msg)
{
    perror(msg);
    exit(0);
}



int main(int argc, char *argv[])
{
    /******************* ESTABLISH IPV4 SOCKET CONNECTION **********************/
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    // check CLI arguments
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    // retrieve port number 
    portno = atoi(argv[2]);

    // create a IPv4 socket 
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    // retrieve server ip 
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    // manage server address 
    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;

    bcopy((char *)server->h_addr_list[0], (char *) &serv_addr.sin_addr.s_addr, server->h_length);
    
    serv_addr.sin_port = htons(portno);
     
    // connect to socket 
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    else 
        printf("Connected to server\n");



    /**************** PARSE TEXT FILE  ****************************/ 

    const unsigned MAX_LENGTH = 256;
    char cmdbuffer[MAX_LENGTH];
    node_t* head = malloc(sizeof(node_t));
    node_t* hp = head; 
    char* sdup; 
    FILE *fp = fopen("commands.txt", "r");
    int cmd_count = 0; 
    char buffer[256];


    // try to open commands.txt
    if (fp == NULL){
        printf("Error: could not open text file");
        return 1;
    }

    // parse it and fill linked list 
    printf("Parsed text file: \n");
    usleep(500000);
    while (fgets(cmdbuffer, MAX_LENGTH, fp)){
        sdup = strdup(cmdbuffer);
        hp->cmd = strtok(sdup, ":");
        hp->value = strtok(0, ":");
        hp->delay = strtok(0, ":");
        hp->next = malloc(sizeof(node_t));
        printf("%s %s %s", hp->cmd, hp->value, hp->delay);
        hp = (node_t*) hp->next;
        cmd_count++;
    }
    hp->next = NULL;
    fclose(fp);



    // Send number of commands to be transfered
    printf("\n");
    printf("\n%d commands will be sent .. \n", cmd_count);

    bzero(buffer,256);
    sprintf(buffer,"%d", cmd_count);

    // send server number of commands 
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
         error("ERROR writing to socket");

    // a bit of delay 
    sleep(1);


    // start sending commands over serial comm. socket 
    hp = head; 
    while(cmd_count--){
       
        printf("Sending .. %s %s %s", hp->cmd, hp->value, hp->delay);

        // send command
        bzero(buffer,256);
        strcpy(buffer, hp->cmd); 
        n = write(sockfd,buffer,strlen(buffer));
        if (n < 0) 
            error("ERROR writing to socket");

        // send command value 
        usleep(100000);
        bzero(buffer,256);
        strcpy(buffer, hp->value); 
        n = write(sockfd,buffer,strlen(buffer));
        if (n < 0) 
            error("ERROR writing to socket");

        // send command delay 
        usleep(100000);
        bzero(buffer,256);
        strcpy(buffer, hp->delay); 
        n = write(sockfd,buffer,strlen(buffer));
        if (n < 0) 
            error("ERROR writing to socket");

        usleep(500000);
        hp = hp->next;
    }

    printf("\n");
    // free alocated memory 
    hp = head;
    while(hp){
        free(hp);
        hp=hp->next;
    }

    close(sockfd);


    // end of main 
    return 0;
}
