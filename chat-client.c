//Carolina de Botton - 87640 | Catarina Goncalves - 90709 | Francisco Guedes - 90716

#include <stdio.h> 
#include <stdlib.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include <netdb.h>

#define MESSAGE_SIZE 4096

   
int main(int argc, char const *argv[]) {

    int server_socket = 0, valread; 
    struct sockaddr_in serv_addr; 
    struct hostent * host;
    char buffer[MESSAGE_SIZE] = {0}; 
    char* address;
    int port;
    int h_addr;
    fd_set client_fd;

    if (argc == 3) {

        if( ( host = gethostbyname(argv[1]) ) == NULL){
            printf("Failed to read host ip \n");
            exit(-1);      
        }

        port = atoi(argv[2]);

       // printf("%s  %d", address, port);
    }

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0){ 
        printf("Failed to initialize client socket \n"); 
        return -1; 
    } 

    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(port); 
    serv_addr.sin_addr = *((struct in_addr *) host->h_addr); 
       
    // Convert IPv4 and IPv6 addresses from text to binary form 
   /* if(inet_pton(AF_INET, host->h_addr, &serv_addr.sin_addr) <= 0){ 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } */
   
    if (connect(server_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){ 
        printf("Failed to connect to server \n"); 
        return -1; 
    } 

    while(1){
        FD_ZERO(&client_fd);
        FD_SET(0, &client_fd);
        FD_SET(server_socket, &client_fd);

        if (select(server_socket + 1, &client_fd, NULL, NULL, NULL) < 0){
            printf("No feeling.\n");   
        } 

        if (FD_ISSET(0, &client_fd)){

            //scanf(" %[^\n]", buffer);
            read(0, buffer, MESSAGE_SIZE);

            if (strlen(buffer) > 0){
                if (write(server_socket, buffer, strlen(buffer)) != strlen(buffer)){
                    perror("Failed to send bye message to clients.");   
                }

            }
            else if (strlen(buffer) == 0 && buffer[strlen(buffer)-1] != EOF){
                exit(0);
            }
            bzero(&buffer, sizeof(buffer));
        }

        else if (FD_ISSET(server_socket, &client_fd)){
            if (recv(server_socket, buffer , MESSAGE_SIZE , 0) == 0){
                exit(0);
            }
            else{
                printf("%s", buffer);
            }
        }

        bzero(&buffer, sizeof(buffer));
        
    }
 
} 


