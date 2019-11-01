//Carolina de Botton - 87640 | Catarina Goncalves - 90709 | Francisco Guedes - 90716
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <string.h>
#include <signal.h> 

#define MAX_CLIENT 1000
#define MESSAGE_SIZE 4096


int server_socket;

void sigintHandler(int sig){
    signal(SIGINT, sigintHandler); 
    close(server_socket);
    exit(0);
}

int main(int argc, char const *argv[]) 
{ 
    char hello_message[MESSAGE_SIZE];
    int valread; 
    int incoming_socket;
    int boss_server;
    int client_sockets[MAX_CLIENT];
    int port;
    struct sockaddr_in address; 
    int opt = 1; 
    int i;
    int addrlen = sizeof(address); 
    char buffer[MESSAGE_SIZE] = {0}; 
    fd_set server_fd;


    for (int i = 0; i < MAX_CLIENT; i++){
        client_sockets[i] = 0;
    }


    if (argc != 2){
        perror("No port defined.");
        exit(-1);
    }
    else{
        port = atoi(argv[1]);
    }  

    // Creating socket file descriptor 
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0){ 
        perror("Socket initialization failed."); 
        exit(-1); 
    } 
    
    //Allows multiple client connections to server socket at once
    if(setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 ){   
        perror("Failed to set socket options.");   
        exit(-1);   
    }   
     
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons(port); 
     
    if (bind(server_socket, (struct sockaddr *)&address, sizeof(address)) <0){ 
        perror("Socket bind failed."); 
        exit(-1); 
    } 
    
    if (listen(server_socket, MAX_CLIENT) < 0){   
        perror("Failed to start listening to the client connections.");   
        exit(-1);   
    }   


    signal(SIGINT, sigintHandler); 

    
         
    //Main Loop
    while(1){
        FD_ZERO(&server_fd);
        FD_SET(server_socket, &server_fd);
        boss_server = server_socket;

        //a gente nao usou uma copia do socket do servidor mas sim alteramos o socket original
        for ( int i = 0 ; i < MAX_CLIENT ; i++){ 
            if (client_sockets[i] > 0){
                FD_SET(client_sockets[i] , &server_fd);
            }  
            if (client_sockets[i] > boss_server){
                boss_server = client_sockets[i];   
            }   
        }   

        if (select(boss_server + 1, &server_fd, NULL, NULL, NULL) < 0){
            printf("No feeling.\n");   
        } 

        if (FD_ISSET(server_socket, &server_fd)){   

            if ((incoming_socket = accept(server_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0){   
                perror("Failed to accept the incoming socket.");   
                exit(-1);   
            }
        
            //add new socket to array of client sockets  
            for (int i = 0; i < MAX_CLIENT; i++){   
                //check client_sockets[i] sockets position is currently free.
                if (client_sockets[i] == 0 ){   
                    client_sockets[i] = incoming_socket;   
                    break;   
                }
            }   

            sprintf(hello_message, "%s:%d joined.\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
            printf("%s", hello_message);
            
            for (int i = 0; i < MAX_CLIENT; i++){
                if(client_sockets[i] != 0){
                    if(write(client_sockets[i], hello_message , strlen(hello_message)) != strlen(hello_message) ){   
                        perror("Failed to send hello message to clients.");   
                    }   
                }
            }

            bzero(&hello_message, sizeof(hello_message));

        }   
             
         //else its some IO operation on some other socket 

        for (int i = 0; i < MAX_CLIENT; i++){
            bzero(&buffer, sizeof(buffer));            
            if (FD_ISSET(client_sockets[i] , &server_fd)){   
                //Check if it was for closing , and also read the incoming message  
                if ((valread = read(client_sockets[i], buffer, MESSAGE_SIZE)) == 0 || strlen(buffer) == 0){   
                    
                    //A client disconnect from the chat room  
                    getpeername(client_sockets[i] , (struct sockaddr*)&address, (socklen_t*)&addrlen);


                    char bye_message[MESSAGE_SIZE];
                    bzero(&bye_message, sizeof(bye_message));
                    sprintf(bye_message, "%s:%d left.\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
                    printf("%s", bye_message);
                    for (int j = 0; j < MAX_CLIENT; j++){
                        if (client_sockets[j] != 0 && j != i){
                            if(write(client_sockets[j], bye_message , strlen(bye_message)) != strlen(bye_message)){   
                                perror("Failed to send bye message to clients.");   
                            }   
                        } 
                    } 
                    //Close the socket and mark as 0 in list for reuse  
                    close(client_sockets[i]);   
                    client_sockets[i] = 0;   
                }      
                
                //Echo back the message that came in  
                else {   
                    //set the string terminating NULL byte on the end  
                    //of the data read
                    getpeername(client_sockets[i] , (struct sockaddr*)&address, (socklen_t*)&addrlen);
                    char message[MESSAGE_SIZE];
                    bzero(&message, sizeof(message));
                    sprintf(message, "%s:%d %s", inet_ntoa(address.sin_addr), ntohs(address.sin_port), buffer);
                    printf("%s", message);
                    for (int j = 0; j < MAX_CLIENT; j++){
                        if (client_sockets[j] != 0 && j != i){
                            if(write(client_sockets[j] , message , strlen(message)) != strlen(message)){   
                                perror("Failed to send a message from the client to others.");   
                            }   
                        } 
                    } 
                }   
            }
        }
    }

}
