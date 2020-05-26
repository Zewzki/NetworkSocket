#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SIZE 1024
#define PORT 16250
#define WEB_PORT "80"
#define IP 128.255.17.20

//buffer to store send and receive data
char buffer[SIZE];

//function definition for charReplacement
void charReplacement(char * filename, char * keyword);

//begin main function
int main(int argc, char * argv[]) {
    
    //confirm user input correct number of args
    if(argc != 3) {
        fprintf(stderr, "usage: %s word to replace, %s server port\n", argv[1], argv[2]);
        exit(1);
    }
    
    //convert args into more user friendly vars
    char * keyword = argv[1];
    int port = atoi(argv[2]);
    
    //declare server socket and client socket
    int sockfd, sockfdClient;
    
    //declare various ints used to assist in program
    int nread, len;
    
    //declare helper structs to assist in connecting
    struct sockaddr_in serverAddress;
    struct sockaddr_in clientAddress;
    
    //define helper struct
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(port);
    
    //create socket
    //exit on error
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket Initialization Failed");
        exit(2);
    }
    
    //bind socket
    //exit on eror
    if(bind(sockfd, (struct sockaddr*) &serverAddress, sizeof(serverAddress)) < 0) {
        perror("Failed to Bind");
        exit(3);
    }
    
    //begin listen phase
    len = sizeof(clientAddress);
    listen(sockfd, 5);
    
    //enter this loop when 
    for(;;) {
        
        //accept connection requests
        //return to listen on failure
        if((sockfdClient = accept(sockfd, (struct sockaddr*) &clientAddress, &len)) == -1) {
            
            perror("Failed to Accept Connection");
            continue;
            
        }
        
        printf("Waiting for file request...\n");
        
        //waits to receive specification on which file to get
        nread = recv(sockfdClient, buffer, SIZE, 0);
        
        //errors out if received data is of 0 size or less
        if(nread <= 0) {
            perror("Error reading from socket\n");
            continue;
        }
        
        printf("%s\n", buffer);
        
        //convert received data into curl command
        //concatenate various strings together
        char *url[512];
        strcat(url, "curl http://user.engineering.uiowa.edu/~jwryan/Communication_Networks/");
        strcat(url, buffer);
        strcat(url, " -o ");
        strcat(url, buffer);
        
        printf("%s\n", url);
        
        //execute as system command
        system(url);
        
        printf("Successfully downloaded %s\n", buffer);
        
        //call function to replace all matching words with ***
        charReplacement(buffer, keyword);
        
        printf("Transmitting altered file back to client\n");
        
        //begin transmission of censored file back to client
        FILE * censoredFile;
        
        //open saved file
        censoredFile = fopen(buffer, "r");
        
        if(censoredFile) {
            
            char textBuffer[SIZE];
            size_t curr;
            
            //send file back as chunks of SIZE size
            while((curr = fread(textBuffer, 1, sizeof(textBuffer), censoredFile)) > 0) {
                
                send(sockfdClient, textBuffer, SIZE, 0);
                
            }
            
            //close file
            
            fclose(censoredFile);
            
            printf("File successfully transmitted\nClosing Connection\n");
            
            //close connection
            close(sockfdClient);
            
        }
        
    }
    
    //system("curl http://user.engineering.uiowa.edu/~jwryan/Communication_Networks/alice.txt -o alice.txt");
    
    
}

//function to convert all words in file matching keyword
//into series of astrices
void charReplacement(char * filename, char * keyword) {
    
    printf("Beginning Word Replacement\n");
    
    FILE * fptr;
    FILE * nptr;
    
    //open saved file and create new file
    fptr = fopen(filename, "r");
    nptr = fopen("new.txt", "w");
    
    //read in file contents and alter them
    
    //printf("Opened both Files\n");
    
    if(fptr && nptr) {
        
        char textBuffer[SIZE];
        size_t curr;
        
        //operate on chunks of size SIZE
        while((curr = fread(textBuffer, 1, sizeof(textBuffer), fptr)) > 0) {
            
            //compare strings of equal size to keyword
            //if they match, replace with ***
            for(int i = 0; i < SIZE - strlen(keyword); i++) {
                
                _Bool isKeyword = 1;
                
                for(int j = 0; j < strlen(keyword); j++) {
                    if(textBuffer[i + j] != keyword[j]) {
                        isKeyword = 0;
                        break;
                    }
                }
                
                if(isKeyword == 1) {
                    for(int j = 0; j < strlen(keyword); j++) {
                        textBuffer[i + j] = '*';
                    }
                }
                
            }
            
            //write edited chunk to new file
            fputs(textBuffer, nptr);
        }
        
        //close files
        fclose(fptr);
        fclose(nptr);
        
        //remove old file and rename new file to take old files place
        
        remove(filename);
        
        rename("new.txt", filename);
        
        printf("Successfully removed all \"%s\"\n", keyword);
        
    }
    else {
        printf("Error opening one or both files");
        return;
    }
    
}

