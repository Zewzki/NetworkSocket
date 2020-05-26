#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#define SIZE 1024
#define PORT 16250
#define WEB_PORT 80

//http://user.engineering.uiowa.edu/~jwryan/Communication_Networks/

//buffer to hold all stored data
char buffer[SIZE];
//main function
int main(int argc, char *argv[]) {
    
    //ensure user input correct number of args
    if(argc != 4) {
        
        fprintf(stderr, "usage: %s filename, %s host, %s port", argv[1], argv[2], argv[3]);
        exit(1);
        
    }
    
    //handel for socket
    int sockfd;
    //number of bytes read back
    int nread;
    
    //helper structs to help connect socket
    struct sockaddr_in serverAddress;
    struct hostent * h;
    
    //convert args into friendlier variable names
    char * filename = argv[1];
    char * host = argv[2];
    int port = atoi(argv[3]);
    
    //create socket
    //exit on error
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        
        perror("Socket Initialization failed");
        exit(2);
        
    }
    
    //init helper structs to connect()
    serverAddress.sin_family = AF_INET;
    h = gethostbyname(argv[2]);
    bcopy(h->h_addr, (char *) &serverAddress.sin_addr, h->h_length);
    serverAddress.sin_port = htons(port);
    
    //connect
    //exit on error
    if(connect(sockfd, (struct sockaddr*) &serverAddress, sizeof(serverAddress)) < 0) {
        perror("Failed to Connect");
        exit(3);
    }
    
    printf("Successfully Connected\n");
    
    //send filename of desired file
    send(sockfd, argv[1], sizeof(argv[1]) + 1, 0);
    
    //create newFile pointer
    //points to file where received data will be stored
    FILE * newFile;
    
    //open new file
    newFile = fopen(filename, "w");
    
    printf("Incoming data...\n");
    
    //as long as port continues to receive data, continue accepting and placing into file
    while(recv(sockfd, buffer, SIZE, 0) > 0){
        fputs(buffer, newFile);
    }
    
    //close file
    fclose(newFile);
    
    //confirm data save location
    printf("Data saved to %s\n", filename);
    
    return 0;
}
