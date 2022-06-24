#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include "chess.h"
#define RETRY_NUMB 8
#define DEFAULT_RECEIVE_SIZE 512
#define MAX_ROOMS 5
#define DESCRIPTION_SIZE 254
#define NAME_SIZE 32

//struct for storing room info
typedef struct room {    
    char server_ip[INET_ADDRSTRLEN];    
    int player_sockets[NUM_OF_PLAYERS];  
    int current_num_of_players;
    char *name;
    char *description;
} room_t;

//=======================================================================================================
//global variables

int bc_port = 51387;
int stream_port = 53820;
//init at start with get local ip
char *device_ip;
room_t current_room = {NULL, NULL, 0, NULL, NULL};

//=======================================================================================================
//helper functions

static void create_room(room_t *lan_room) {
    lan_room->current_num_of_players = 1;
    lan_room->description = calloc(DESCRIPTION_SIZE, 1);
    lan_room->name = calloc(NAME_SIZE, 1);
}

static void free_room(room_t *lan_room) {
    free(lan_room->name);
    free(lan_room->description);
}

static bool add_player_to_room(int player_socket, room_t *current_room) {
    if (current_room->current_num_of_players < 3) {
        current_room->player_sockets[current_room->current_num_of_players] = player_socket;        
        current_room->current_num_of_players++;
        return true;
    } else {
        return false;
    }
}

static bool setup_send_broadcast(int *bc_socket, struct sockaddr_in *bc_addr,int port_num, u_int32_t receiver_ip) {
    bc_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (bc_socket < 0) {
        printf("failed to get broadcast socket\n");
        //send singnal to exit lobby
        return false;
    }

    int bc_permission = 1;
    int s_soc = setsockopt(bc_socket, SOL_SOCKET, SO_BROADCAST, &bc_permission, sizeof(bc_permission));
    if (s_soc < 0) {
        printf("failed to set broadcast socket\n");
        close(bc_socket);
        //send error to exit lobby
        return false;
    }

    bc_addr->sin_addr.s_addr = htonl(receiver_ip);
    bc_addr->sin_port = htons(bc_port);
    bc_addr->sin_family = AF_INET;
}

static bool setup_listen_broadcast(int *listen_socket, int port_num, int timeout) {
    listen_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (listen_socket < 0) {
        printf("failed to get listen socket\n");
        return false;
    }

    if (timeout != -1) {
        struct timeval time_out;
        time_out.tv_sec = timeout;
        time_out.tv_usec = 0;
        if (setsockopt(listen_socket, SOL_SOCKET, SO_RCVTIMEO, &time_out, size(time_out)) < 0) {
            printf("failed to set listen_socket\n");
            close(listen_socket);
            return false;
        }
    }

    struct sockaddr_in host_addr;
    
    memset(&host_addr, 0, sizeof(host_addr));
    host_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    host_addr.sin_family = AF_INET;
    host_addr.sin_len = sizeof(host_addr);
    host_addr.sin_port = htons(port_num);

    int status = bind(listen_socket, (struct sockaddr*)&host_addr, sizeof(host_addr));
    if (status < 0) {
        printf("failed to bind socket\n");
        close(listen_socket);
        return false;
    }
    return true;
}

static bool get_broadcast_msg(int *listen_socket, char *msg, int msg_size, struct sockaddr_in *sender_addr) {
    int sender_addr_len = sizeof(sender_addr);
    ssize_t received_size = recvfrom(listen_socket, msg, msg_size, 0, (struct sockaddr *)&sender_addr, &sender_addr_len);
    if (received_size = 0) {
        printf("no broadcasst received\n");
        return false;
    }
    return true;
}

static bool connect_to_stream_server(char *server_ip, int port_num, int *client_socket) {
    client_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        printf("failed to get client socket\n");
        return false;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_addr.s_addr = htonl(server_ip);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_num);

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("connection failed\n");
        return false;
    } 
    return true;
}

static bool setup_stream_server(int *server_socket, int port_num) {
    server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket < 0) {
        printf("failed to get socket\n");
        return false;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST); 
    server_addr.sin_port = htons(port_num);

    bind(server_socket, (struct sockaddr*)&server_addr,sizeof(server_addr));

    if(listen(server_socket, 10) == -1){
        printf("listen failed");
        return false;
    }
    return true;
}

//=======================================================================================================
//lan connections

bool enter_lobby() {
    //send socket

    int bc_socket;
    struct sockaddr_in bc_addr;
    memset(&bc_addr, 0, sizeof(bc_addr));
    setup_send_broadcast(&bc_socket, &bc_addr, bc_port, INADDR_BROADCAST);

    char *bc_msg = "searching chess game";

    //receive socket

    int listen_socket;
    setup_listen_broadcast(&listen_socket, bc_port, 3);

    struct sockaddr_in sender_addr;
    socklen_t sender_addr_len = sizeof(sender_addr);
    char server_ip[MAX_ROOMS][INET_ADDRSTRLEN];
    char *room_info[MAX_ROOMS];
    int num_rooms = 0;
    
    //send and receive while loop
    //get host ip and room info
    while (1) {
        //send info to display to display rooms


        //receive from display to join room(by search or explicit ip), refresh or exit lobby



        //refresh
        int send_bc = sendto(bc_socket, bc_msg, strlen(bc_msg), 0, (struct sockaddr*)&bc_addr, sizeof(bc_addr));
        if (send_bc < 0) {
            printf("unable to send broadcast\n");
            close(bc_socket);
            return false;
        }
        for (int i = 0; i != MAX_ROOMS; i++) {
            ssize_t received_size;
            get_broadcast_msg(listen_socket, room_info[num_rooms], DEFAULT_RECEIVE_SIZE, &sender_addr);
            if (received_size > 0 ) {
                //if received the broadcast msg
                if (room_info[num_rooms] == bc_msg) {
                    i--;
                    continue;
                }
                if (inet_ntop(AF_INET, &sender_addr.sin_addr, server_ip[num_rooms], (size_t)sizeof(server_ip[num_rooms])) == NULL) {
                    server_ip[num_rooms][0] = '\0';
                } else {
                    num_rooms++;
                }
            }
        }


    }
    return true;
}

bool start_hosting() {
    int all_sockets[2];

    //create room
    create_room(&current_room);

    //stream socket for lan communication
    int server_socket;
    if (!setup_stream_server(server_socket, stream_port)) {
        return false;
    }

    //broadcast sockets to make the room visible to players
    int host_listen_socket;
    if(!setup_listen_broadcast(&host_listen_socket, bc_port, -1)) {
        return false;
    }
    char *buffer = calloc(DEFAULT_RECEIVE_SIZE, 1);


    //listen to both sockets at the same time    
    //listen to player broadcast 
    //sent player info of the room
    //listen to any stream accept (add player to room)    
    //send display about the update
    //listen to any player change stat: ready/waiting
    //send player about connection 

    struct sockaddr_in sender_addr;
    all_sockets[0] = server_socket;
    all_sockets[1] = host_listen_socket;
    int select_return;
    fd_set read_fd_set;
    while (current_room.current_num_of_players < 3) {
        FD_ZERO(&read_fd_set);
        for (int i = 0; i != 2; i++) {
            if (all_sockets[i] >= 0) {
                FD_SET(all_sockets[i], &read_fd_set);
            }
        }

        //wait for msg from any port
        select_return = select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL);

        //receive
        if (select_return >= 0) {
            //for new user joining room
            if (FD_ISSET(server_socket, &read_fd_set)) {
                struct sockaddr_in player_addr;
                int player_addr_len = sizeof(player_addr);
                int player_socket = accept(server_socket, (struct sockaddr*)&player_addr, &player_addr_len);
                if (player_socket >= 0) {
                    add_player_to_room(player_socket, &current_room);
                }
                continue;
            }
        }      

    }
    


    if (!get_broadcast_msg(host_listen_socket, buffer, DEFAULT_RECEIVE_SIZE, &sender_addr)) {
        return false;
    }
    


}

bool join_room(char *host_ip, int *socket) {
    connect_to_stream_server(host_ip, stream_port, &socket);
    return true;
}

bool leave_room(int player_socket) {
    close(player_socket);
    
    return true;
}

bool host_close_room() {
    for (int i = 0; i != current_room.current_num_of_players; i++) {
        close(current_room.player_sockets[i]);
    }
    free_room(&current_room);
    return true;
}
