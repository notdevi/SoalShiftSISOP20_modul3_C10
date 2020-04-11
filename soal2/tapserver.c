#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#define PORT 4444

int main(int argc, char const *argv[]) {
    int server_fd, new_socket, valread, valread2;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
  
    char *msg = "penambahan berhasil", *data;
      
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
      
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
      
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    
    FILE* file_ptr;

    // int count = 0;
    while(1){
        file_ptr = fopen("akun.txt", "a");
        char buffer[1024] = {0};
        char buffer2[1024] = {0};
        char buffer3[1024] = {0};

        valread = read( new_socket , buffer, 1024);     
        
        if(strcmp(buffer, "register")==0){
            valread = read( new_socket , buffer2, 1024);

            valread2 = read( new_socket , buffer3, 1024);

            fputs(buffer2, file_ptr);
            fputs(buffer3, file_ptr);
            fclose(file_ptr);
        }
        else if(strcmp(buffer, "login")==0){
            valread = read( new_socket , buffer2, 1024);
            printf("%s\n", buffer2);
        }
    }
    
    return 0;
}
