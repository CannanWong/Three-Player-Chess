#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include "chess.h"

//global variables

int server_socket = 0;
int display_socket = 0;
int client_socket[NUM_OF_PLAYERS] = {0};

int display_port = 500;
//multiplayer port = display port + 1
char *local_ip;
struct sockaddr_in server_addr;
struct sockaddr_in display_addr;
unsigned int display_size = sizeof(display_addr);

static char *get_local_ip() {
    char buffer[256];
    struct hostent *local_info;
    gethostname(buffer, sizeof(buffer));
    local_info = gethostbyname(buffer);
    return inet_ntoa(*((struct in_addr*)local_info->h_addr_list[0]));
}

void start_server() {
    memset(&server_addr, '0', sizeof(server_addr));

    //get socket
    server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
    server_addr.sin_port = htons(display_port);

    bind(server_socket, (struct sockaddr*)&server_addr,sizeof(server_addr));

    printf("loop\n");
    for (int i = 0; i != 30; i++) {
        if(listen(server_socket, 10) == -1){
            printf("listen failed");
            exit(EXIT_FAILURE);
        } else {
            break;
        }
    }

    printf("trying to connect\n");
    display_socket = accept(server_socket, (struct sockaddr*)&display_addr, &display_size);
}

void close_display() {
    char *msg_close_display;
    bool close_success = false;
    for(int i = 0; i != 30; i++) {
        if (send(display_socket, msg_close_display, sizeof(msg_close_display), 0) == sizeof(msg_close_display)) {
            close_success = true;
            break;
        }
    }
    if (!close_success) {
        fprintf(stderr, "close display unsuccessful");
    }
    close(display_socket);
}

bool receive_msg(char *msg, int max_size) {
    int msgsize = 0;
    return (msgsize = recv(display_socket, msg, max_size, 0)) < 0;
}

bool send_msg(char *msg, int max_size) {
    return (send(display_socket, msg, max_size, 0)) != max_size;
}

int main() {
    start_server();
    char receive[40], send[40];

    receive_msg(receive, sizeof(receive));
    printf("receive: %s", receive);

    scanf("%40s", send);
    send_msg(send, sizeof(send));

    close_display();
}
