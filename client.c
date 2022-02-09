/* Client */



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
    int delay; 
    struct node* next;
}node_t;

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{

    // variables for internet socket 
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];

    // variable for parsing text file 
    const unsigned MAX_LENGTH = 256;
    char cmdbuffer[MAX_LENGTH];
    node_t* head = malloc(sizeof(node_t));
    node_t* hp = head; 
    char* sdup; 
    FILE *fp = fopen("commands.txt", "r");
    int cmd_count = 0; 

    // try to open commands.txt
    if (fp == NULL){
        printf("Error: could not open text file");
        return 1;
    }

    // parse it and fill linked list 
    while (fgets(cmdbuffer, MAX_LENGTH, fp)){
        sdup = strdup(cmdbuffer);
        hp->cmd = strtok(sdup, ":");
        hp->value = strtok(0, ":");
        hp->delay = atoi(strtok(0, ":"));
        hp->next = malloc(sizeof(node_t));
        hp = (node_t*) hp->next;
    }
    hp->next = NULL; 



    fclose(fp);

    /******************************************************/


    // Checks if app is run with proper arguments 
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    // retrieves port number 
    portno = atoi(argv[2]);

    // create a IPv4 socket 
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    // retrieves server address from CLI arguments
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    // erases the n bytes of memory starting at memory location of first argument
    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;

    // copies n bytes from src to dest 
    bcopy((char *)server->h_addr_list[0], (char *) &serv_addr.sin_addr.s_addr, server->h_length);
    
    serv_addr.sin_port = htons(portno);
     
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");


    /**************** CONNECTION ESTABLISHED ****************************/ 


    hp = head; 
    while(hp){
        printf("Tokenized: %s %s %d\n", hp->cmd, hp->value, hp->delay);
        //free(hp);
        hp = hp->next;
        cmd_count++;
    }
    // from 6 to 5
    cmd_count = cmd_count-1;



    // Send number of commands to be transfered
    printf("Commands to be sent: %d\n", cmd_count);
    bzero(buffer,256);
    sprintf(buffer,"%d", cmd_count);
    // send server number of commands 
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
         error("ERROR writing to socket");

    hp = head; 


    // start sending commands 
    while(cmd_count--){
       
        // erases buffer
        bzero(buffer,256);

        // fill buffer with standard input 
        // fgets(buffer,255,stdin);
        //strcpy(buffer, "lalal");
        strcpy(buffer, hp->cmd);
        hp = hp->next;
        //free(hp)
        printf("Sending cmd ..%s\n", buffer);

        // write to pipe
        n = write(sockfd,buffer,strlen(buffer));
        if (n < 0) 
            error("ERROR writing to socket");

        sleep(1);

    }
   

 

    close(sockfd);
    return 0;
}
