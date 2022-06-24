#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include "chess.h"
#define RETRY_NUMB 8
#define DEFAULT_RECEIVE_SIZE 512
#define MAX_ROOMS 5
#define DESCRIPTION_SIZE 254
#define NAME_SIZE 32

//=======================================================================================================
//global variables

int server_socket = 0;
int display_socket = 0;


//=======================================================================================================
//helper functions

char *get_local_ip() {
    char buffer[256];
    struct hostent *local_info;
    gethostname(buffer, sizeof(buffer));
    local_info = gethostbyname(buffer);
    return inet_ntoa(*((struct in_addr*)local_info->h_addr_list[0]));
}
/*
static bool connect_to_stream_server_(char *server_ip, int port_num, int *client_socket) {
    struct sockaddr_in server_addr;

    *client_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (*client_socket < 0) {
        printf("failed to get client socket\n");
        return false;
    }

    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_num);

    if (connect(*client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("connection failed\n");
        return false;
    } 
    return true;
}
*/
static bool setup_stream_server(int *server_socket, int port_num) {
    *server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (*server_socket < 0) {
        printf("failed to get socket\n");
        return false;
    }
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST); 
    server_addr.sin_port = htons(port_num);

    bind(*server_socket, (struct sockaddr*)&server_addr,sizeof(server_addr));

    if(listen(*server_socket, 10) == -1){
        printf("listen failed");
        return false;
    }


    return true;
}

//=======================================================================================================
//display connection

bool start_server() {
    //get socket
    setup_stream_server(&server_socket, display_port);
    struct sockaddr_in display_addr;
    unsigned int display_size = sizeof(display_addr);
    display_socket = accept(server_socket, (struct sockaddr*)&display_addr, &display_size);

    return true;
}

bool close_display() {
    char *msg_close_display = "close";
    if (send(display_socket, msg_close_display, sizeof(msg_close_display), 0) != sizeof(msg_close_display)) {
        printf("failed to request close display\n");
        return false;
    }
    close(display_socket);
    return true;
}

bool receive_msg(char *msg, int size) {
    return recv(display_socket, msg, size, 0) < 0;
}

bool send_msg(char *msg, int size) {
    return (send(display_socket, msg, size, 0)) != size;
}

