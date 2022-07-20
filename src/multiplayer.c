#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <stdbool.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <ctype.h>
#include "chess.h"

#define SUBNET_MASK 0xffffff00
#define NUM_OF_PLAYERS 2
#define RETRY_NUMB 8
#define DEFAULT_RECEIVE_SIZE 512
#define MAX_ROOMS 5
#define DESCRIPTION_SIZE 254
#define NAME_SIZE 32
#define NUMB_OF_SERVER_SOCKETS 4
#define MAX_MSG_SIZE 1024
#define PLAYER_INFO_SIZE NUM_OF_PLAYERS*(NAME_SIZE + 1)

int display_socket = 0;
bool receive_display_msg(void *msg, int size) {
    if (recv(display_socket, msg, size, 0) > 0) {
        return true;
    } else {
        printf("failed to recv display msg\n");
        return false;
    }
}

bool send_display_msg(void *msg, int size) {
    //printf("sending: %i %i %i\n", msg[0], msg[1], msg[2]);
    //printf("sending: %i %i %i\n", msg[3], msg[4], msg[5]);
    if ((send(display_socket, msg, size, 0)) == size) {
        return true;
    } else {
        printf("failed to send msg\n");
        return false;
    }
}

//=======================================================================================================
//data structures

//struct for player
typedef struct lobby_player {
    bool ready;
    int socket_number;
    char *name;
} lobby_player_t;

//struct for storing room info
typedef struct room {    
    int current_num_of_players;    
    char server_ip[INET_ADDRSTRLEN];    
    lobby_player_t *players;  
    //room name doesnt contain "|""
    char *room_name;
    char *description;
} room_t;

//=======================================================================================================
//global variables

const int bc_port = 51387;
const int stream_port = 53820;
//init at start with get local ip
char device_ip[INET_ADDRSTRLEN] = {0};
const char *fail = "0";
const char *success = "1";
const char *connection_msg = "jesh_andy_jiaju_cannan";

//=======================================================================================================
//helper functions

static void create_room(room_t *lan_room, char *name, char *description) {
    lan_room->current_num_of_players = 0;
    lan_room->description = calloc(DESCRIPTION_SIZE, 1);
    lan_room->room_name = calloc(NAME_SIZE, 1);
    lan_room->players = calloc(NUM_OF_PLAYERS * sizeof(lobby_player_t), 1);
    strcpy(lan_room->server_ip, device_ip);
    strcpy(lan_room->room_name, name);
    strcpy(lan_room->description, description);
}

static void free_room(room_t *lan_room) {
    for (int i = 0; i < NUM_OF_PLAYERS; i++) {
        if (lan_room->players[i].name != NULL) {
            free(lan_room->players[i].name);
        }
    }
    free(lan_room->players);
    free(lan_room->room_name);
    free(lan_room->description);
}

static bool add_player_to_room(room_t *current_room, int player_socket) {
    if (current_room->current_num_of_players <= NUM_OF_PLAYERS) {
        int next_index = NUM_OF_PLAYERS;
        for (int i = 0; i < NUM_OF_PLAYERS; i++) {
            if (current_room->players[i].socket_number == 0) {
                next_index = i;
            }
        }
        current_room->players[next_index].ready = false;       
        current_room->players[next_index].socket_number = player_socket; 
        current_room->players[next_index].name = calloc(NAME_SIZE, 1);
        current_room->current_num_of_players++;
        return true;
    } else {
        return false;
    }
}

static bool remove_from_room(room_t *current_room, int player_socket) {
    printf("remove from room\n");
    for (int i = 0; i < NUM_OF_PLAYERS; i++) {
        if (current_room->players[i].socket_number == player_socket) {
            current_room->players[i].socket_number = 0;
            free(current_room->players[i].name);
            current_room->players[i].name = NULL;
            current_room->current_num_of_players--;
            return true;
        }
    }
    return false;
}

static bool get_player_from_room(room_t *current_room, int player_socket, lobby_player_t **player) {
    for (int i = 0; i != NUM_OF_PLAYERS; i++) {
        if (current_room->players[i].socket_number == player_socket) {
            *player = &current_room->players[i];
            return true;
        }
    }
    return false;
}

static bool setup_send_broadcast(int *bc_socket, struct sockaddr_in *bc_addr,int port_num, u_int32_t receiver_ip) {
    *bc_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (*bc_socket < 0) {
        printf("failed to get broadcast socket\n");
        //send singnal to exit lobby
        return false;
    }

    int bc_permission = 1;
    if (setsockopt(*bc_socket, SOL_SOCKET, SO_BROADCAST, &bc_permission, sizeof(bc_permission))) {
        printf("failed to set broadcast socket\n");
        close(*bc_socket);
        //send error to exit lobby
        return false;
    }

    bc_addr->sin_addr.s_addr = htonl(receiver_ip);
    bc_addr->sin_port = htons(bc_port);
    bc_addr->sin_family = AF_INET;
    return true;
}

static bool setup_broadcast(int *listen_socket, int port_num, int timeout) {
    *listen_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (*listen_socket < 0) {
        printf("failed to get listen socket\n");
        return false;
    }

    int broadcast = 1;
    if (setsockopt(*listen_socket, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0) {
        printf("failed to set listen_socket broadcast\n");
        close(*listen_socket);
        return false;
    }
    if (timeout != -1) {
        struct timeval time_out;
        time_out.tv_sec = timeout;
        time_out.tv_usec = 0;
        if (setsockopt(*listen_socket, SOL_SOCKET, SO_RCVTIMEO, &time_out, sizeof(time_out)) < 0) {
            printf("failed to set listen_socket\n");
            close(*listen_socket);
            return false;
        }
    }

    struct sockaddr_in host_addr;
    
    memset(&host_addr, 0, sizeof(host_addr));
    host_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(port_num);

    int status = bind(*listen_socket, (struct sockaddr*)&host_addr, sizeof(host_addr));
    if (status < 0) {
        printf("failed to bind socket\n");
        close(*listen_socket);
        return false;
    }
    return true;
}

static bool get_broadcast_msg(int *listen_socket, char *msg, int msg_size, struct sockaddr_in *sender_addr) {
    socklen_t sender_addr_len = sizeof(*sender_addr);
    ssize_t received_size = 0;
    receive_broadcast:
    received_size = recvfrom(*listen_socket, msg, msg_size, 0, (struct sockaddr *)sender_addr, &sender_addr_len);
    if (received_size < 0) {
        return false;
    }
    char *sender_ip = calloc(INET_ADDRSTRLEN,1);
    inet_ntop(AF_INET, &sender_addr->sin_addr, sender_ip, INET_ADDRSTRLEN);
    if (strcmp(sender_ip, device_ip) == 0) {    
        free(sender_ip);
        memset(msg, 0, MAX_MSG_SIZE);
        goto receive_broadcast;
    }
    free(sender_ip);
    return true;
}

static bool connect_to_stream_server(char *server_ip, int port_num, int *client_socket) {
    *client_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (*client_socket < 0) {
        printf("failed to get client socket\n");
        return false;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_num);

    if (connect(*client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("connection failed\n");
        return false;
    } 
    return true;
}

static bool setup_stream_server(int *server_socket, int port_num) {
    *server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (*server_socket < 0) {
        printf("failed to get socket\n");
        return false;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
    server_addr.sin_port = htons(port_num);

    if (bind(*server_socket, (struct sockaddr*)&server_addr,sizeof(server_addr)) < 0) {
        printf("failed to bind socket\n");
        close(*server_socket);
        return false;
    }

    if(listen(*server_socket, 10) == -1){
        printf("listen failed");
        return false;
    }
    return true;
}

static bool join_room(char *host_ip, int *socket, int display_socket) {
    if (!connect_to_stream_server(host_ip, stream_port, socket)) {
        return false;
    } 

    //recieve room info: socket_number|server_ip|player_infos
    char room_info[MAX_MSG_SIZE];
    int sock_recv = recv(*socket, room_info, MAX_MSG_SIZE, 0);
    if (sock_recv < 0) {
        printf("failed to receive\n");
        return false;
    } else if (sock_recv == 0) {
        printf("lost connection to host\n");
        return false;
    }

    //ask display for name
    send_display_msg("name:", 6);

    //receive name    
    char name[NAME_SIZE];
    receive_display_msg(name, NAME_SIZE);

    //send player name
    if (send(*socket, name, NAME_SIZE, 0) < 0) {
        return false;
    }
    send_display_msg(room_info, strlen(room_info));

    fd_set socket_set;
    printf("entered\n");
    while (1) {
        //for testing so display can send msg
        send_display_msg("pppp\n", 6);

        char msg[MAX_MSG_SIZE];
        FD_ZERO(&socket_set);
        FD_SET(display_socket, &socket_set);
        FD_SET(*socket, &socket_set);
        int highest_socket = display_socket;
        if (display_socket < *socket) {
            highest_socket = *socket;
        }
        int ret = select(highest_socket, &socket_set, NULL, NULL, NULL);
        if ( ret > 0) {
            if (FD_ISSET(*socket, &socket_set) > 0) {
                int return_val = recv(*socket, msg, MAX_MSG_SIZE, 0);
                if (return_val > 0) {
                    printf("from server %s\n", msg);
                } else if (return_val == 0) {
                    printf("close socket\n");
                    close(*socket);
                    break;
                } else {
                    printf("no msg received\n");
                }
            }
            if (FD_ISSET(display_socket, &socket_set) > 0) {
                int return_val = recv(display_socket, msg, MAX_MSG_SIZE, 0);
                if (return_val > 0) {
                    printf("from display\n");
                    send(*socket, msg, MAX_MSG_SIZE, 0);
                } else if (return_val == 0) {
                    printf("close socket\n");
                    close(display_socket);
                    break;
                } else {
                    printf("no msg received\n");
                }
                
            }
        }
    } 
    return true;
}

static bool read_room_info(char *room_info, room_t *current_room) {
    char *name_buffer = strtok(room_info, "|");
    char *descript_buffer = strtok(NULL, "|");
    if (descript_buffer != NULL) {   
        char *numb_of_players = strtok(NULL, "|");
        if (numb_of_players != NULL) {
            create_room(current_room, name_buffer, descript_buffer);
            current_room->current_num_of_players = atoi(numb_of_players);
            assert(current_room->current_num_of_players <= 2);
            return true;
        }
    }
    return false;
}

static bool host_send_msg(char *msg, int msg_len, lobby_player_t players[NUM_OF_PLAYERS]) {
    assert(msg_len != 0);
    for (int i = 0; i < NUM_OF_PLAYERS; i++) {
        if (players[i].socket_number != 0) {
            if (send(players[i].socket_number, msg, msg_len, 0) <= 0) {
                return false;
            }
        }
    }
    return true;
}

static char *player_to_string(lobby_player_t players[NUM_OF_PLAYERS]) {
    char *player_info = calloc(PLAYER_INFO_SIZE, 1);
    for (int i = 0; i != NUM_OF_PLAYERS; i++) {
        //player_infos: name|1/0
        if (players[i].socket_number != 0) {
            strcat(player_info, players[i].name);
            strcat(player_info, "|");
            if (players[i].ready) {
                strcat(player_info, "1");
            } else {
                strcat(player_info, "0");
            }
            strcat(player_info, "|"); 
        } else {
            strcat(player_info, "||");
        }
    }
    return player_info;
}

//=======================================================================================================
//lan connections

void host_update(char *update_msg, int msg_len, int display_socket, bool to_players,lobby_player_t players[NUM_OF_PLAYERS]) {
    if (!send_display_msg(update_msg, msg_len)) {
        printf("failed to update display\n");
    }
    if (to_players) {
        if (!host_send_msg(update_msg, msg_len, players)) {
            printf("failed to update players");
        }
    }
    free(update_msg);
}

bool host_close_room(room_t *current_room) {
    for (int i = 0; i != current_room->current_num_of_players; i++) {
        close(current_room->players[i].socket_number);
    }
    free(current_room->room_name);
    free(current_room->description);
    return true;
}

bool start_hosting(room_t *current_room, int host_listen_socket, int display_socket) {
    //stream socket for lan communication
    int server_socket;
    if (!setup_stream_server(&server_socket, stream_port)) {
        return false;
    }
    char *buffer = calloc(DEFAULT_RECEIVE_SIZE, 1);
    printf("host setup listen\n");

    //broadcast addr to reply to player, allow player to know the ip of host
    struct sockaddr_in reply_broadcast_addr;
    reply_broadcast_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    reply_broadcast_addr.sin_port = htons(bc_port);
    reply_broadcast_addr.sin_family = AF_INET;
    
    //int player_sockets[NUM_OF_PLAYERS] = {0};
    struct sockaddr_in sender_addr;
    fd_set socket_set;
    char *msg = malloc(MAX_MSG_SIZE);
    printf("before loop\n");
    while (1) {
        send_display_msg("loop\n", 5);
        memset(msg, 0, MAX_MSG_SIZE);
        FD_ZERO(&socket_set);
        FD_SET(display_socket, &socket_set);
        FD_SET(server_socket, &socket_set);
        FD_SET(host_listen_socket, &socket_set);
        //msg from display for the host to leave;

        int highest_socket = server_socket;
        if (host_listen_socket > server_socket) {
            highest_socket = host_listen_socket;
        }
        for (int i = 0; i < NUM_OF_PLAYERS; i++) {
            if (current_room->players[i].socket_number != 0) {
                FD_SET(current_room->players[i].socket_number, &socket_set);
                if (current_room->players[i].socket_number > highest_socket) {
                    highest_socket = current_room->players[i].socket_number;
                }
            }
        }
        
        //wait for msg from any port
        if (select(highest_socket + 1, &socket_set, NULL, NULL, NULL) >= 0) {
            //send to display and other players about player ready, not ready, leave room, join room update

            //for new player joining room
            if (FD_ISSET(server_socket, &socket_set)) {
                printf("new player joined\n");
                struct sockaddr_in player_addr;
                socklen_t player_addr_len = sizeof(player_addr);
                int player_socket = accept(server_socket, (struct sockaddr*)&player_addr, &player_addr_len);
                if (player_socket >= 0) {                        
                    //if room is full
                    if (!add_player_to_room(current_room, player_socket)) {
                        printf("room is full\n");
                        close(player_socket);
                        continue;
                    }
                    //send room info: server_ip|player_infos
                    char room_info[MAX_MSG_SIZE] = {0};
                    strcat(room_info, current_room->server_ip);
                    strcat(room_info, "|");
                    char *player_info;
                    player_info = player_to_string(current_room->players);
                    strcat(room_info, player_info);
                    if (send(player_socket, room_info, MAX_MSG_SIZE, 0) < 0) {
                        printf("failed to send room infos\n");
                        close(player_socket);
                        continue;
                    }
                    //send msg to display to update
                    //format: player_name|1/0
                    host_update(player_info, strlen(player_info), display_socket, true, current_room->players);                    
                    printf("socket added\n"); 
                }
            }
            //tell player the existance of the room
            else if (FD_ISSET(host_listen_socket, &socket_set)) {
                struct sockaddr_in player_addr;
                socklen_t player_addr_len = sizeof(player_addr);
                if (recvfrom(host_listen_socket, msg, MAX_MSG_SIZE, 0, (struct sockaddr *)&player_addr, &player_addr_len) > 0) {
                    //check if message is from another 3 player chess exe
                    if (strcmp(msg, connection_msg) != 0) {
                        //not a user from 3 player chess
                        continue;
                    }
                    //get player ip
                    char player_ip[INET_ADDRSTRLEN];
                    if (inet_ntop(AF_INET, &player_addr.sin_addr, player_ip, (size_t)sizeof(player_ip)) != NULL) {
                        inet_pton(AF_INET, player_ip, &reply_broadcast_addr.sin_addr);
                        //room info for display: list of players, room name
                        //room display
                        memset(msg, 0, MAX_MSG_SIZE);
                        strcat(msg, current_room->room_name);
                        strcat(msg, "|");
                        strcat(msg, current_room->description);
                        strcat(msg, "|");
                        char *str_numb = malloc(1);
                        sprintf(str_numb, "%d", current_room->current_num_of_players);
                        strcat(msg, str_numb);
                        free(str_numb);
                        printf("send msg from host: %s\n", msg);
                        if (sendto(host_listen_socket, msg, strlen(msg), 0, (struct sockaddr*)&reply_broadcast_addr, sizeof(reply_broadcast_addr)) < 0) {
                            printf("failed to send\n");
                        }

                    }
                }
            } 
            //msg from display
            else if (FD_ISSET(display_socket, &socket_set)) {
                //display msg:  kick:  0|player_index
                //              start: 1
                //              leave: 2
                char display_msg[MAX_MSG_SIZE] = {0};
                if (!receive_display_msg(display_msg, MAX_MSG_SIZE)) {
                    continue;
                }
                char *msg_type;
                msg_type = strtok(display_msg, "|");
                assert(msg_type != NULL);
                switch (msg_type[0])
                {
                case '0':
                    msg_type = strtok(NULL, "|");
                    if (msg_type == NULL) {
                        printf("incorrect msg format\n");
                        break;
                    }
                    if(!isdigit(msg_type[0])) {
                        printf("incorrect msg format\n");
                        break;
                    }
                    int player_index = atoi(msg_type);
                    if (player_index >= NUM_OF_PLAYERS) {
                        printf("player index value too large\n");
                        break;
                    }
                    printf("kick\n");
                    assert(remove_from_room(current_room, current_room->players[player_index].socket_number));
                    break;
                case '1':
                    for (int i = 0; i < NUM_OF_PLAYERS; i++) {
                        if (current_room->players[i].socket_number == 0) {
                            // send msg to display: not enough players?
                            printf("not enough players");
                            goto players_not_ready;
                        }
                        if (!current_room->players[i].ready) {
                            printf("player: %i not ready", i);
                            goto players_not_ready;
                        }
                    }
                    free(msg);
                    //send players update
                    printf("start\n");
                    return true;
                    players_not_ready:
                    printf("not start\n");
                    break;
                case '2':
                    printf("close room\n");
                    host_close_room(current_room);
                    free(msg);
                    return false;
                default:
                    printf("incorrect msg\n");
                    return false;
                    break;
                }
                

            }
            //status change from player
            else {
                bool update = false;
                for (int i = 0; i < NUM_OF_PLAYERS; i++) {
                    if (current_room->players[i].socket_number != 0 && FD_ISSET(current_room->players[i].socket_number, &socket_set)) {
                        int recv_ret = recv(current_room->players[i].socket_number, msg, MAX_MSG_SIZE, 0);
                        if (recv_ret < 0) {
                            continue;
                        } else if (recv_ret == 0) {
                            printf("remove socket\n");
                            //remove socket
                            if (!remove_from_room(current_room, current_room->players[i].socket_number)) {
                                printf("failed to remove from room\n");
                            }
                            continue;
                        }
                        update = true;
                        printf("status change\n");
                        lobby_player_t *current_player;
                        if (!get_player_from_room(current_room, current_room->players[i].socket_number, &current_player)) {
                            printf("player not found\n");
                            continue;
                        }
                        if (msg[0] == '|') {
                            //if msg is for setting player name
                            printf("get name: %s\n", msg + 1);
                            if (current_player == NULL) {
                                printf("current+player is null\n");
                            }
                            if (current_player->name == NULL) {
                                printf("name is null %i\n", current_player->socket_number);
                            }
                            strncpy(current_player->name, msg + 1, strlen(msg) - 1);
                        } else {
                            if (strcmp(msg, "ready") == 0) {
                                printf("ready\n");
                                current_player->ready = true;
                            } else if (strcmp(msg, "cancel") == 0) {
                                printf("cancel\n");
                                current_player->ready = false;
                            } else if (strcmp(msg, "leave") == 0) {    
                                printf("leave\n");
                                close(current_room->players[i].socket_number);
                                //remove socket
                                if (!remove_from_room(current_room, current_room->players[i].socket_number)) {
                                    printf("failed to remove\n");
                                }
                            } else {
                                //wrong msg
                                printf("incorrect msg: %s\n", msg);
                                send(current_room->players[i].socket_number, fail, sizeof(fail), 0);  
                                continue;  
                            }
                        }
                        send(current_room->players[i].socket_number, success, sizeof(success), 0);      
                    }
                }
                if (update) {
                    host_update(player_to_string(current_room->players), PLAYER_INFO_SIZE, display_socket, true, current_room->players);
                }
            }
            //sleep(1);
        }
    }
    free(msg);
    //enter game
    return true;
}

room_t *enter_lobby(int display_socket) {
    //stream connection socket
    int stream_socket;

    //send socket
    int bc_socket;
    struct sockaddr_in bc_addr;
    memset(&bc_addr, 0, sizeof(bc_addr));
    setup_send_broadcast(&bc_socket, &bc_addr, bc_port, INADDR_BROADCAST);
    //receive socket
    int listen_socket;
    setup_broadcast(&listen_socket, bc_port, 1);

    struct sockaddr_in sender_addr;
    memset(&sender_addr, '0', sizeof(sender_addr));
    socklen_t sender_addr_len = sizeof(sender_addr);
    char *read_buffer = malloc(MAX_MSG_SIZE);

    //send and receive while loop
    //get host ip and room info
    while (1) {
        room_t *room_infos[MAX_ROOMS] = {NULL};    
        memset(read_buffer, 0, MAX_MSG_SIZE);
        //send to display to display rooms
        int send_bc = sendto(listen_socket, connection_msg, strlen(connection_msg), 0, (struct sockaddr*)&bc_addr, sizeof(bc_addr));        
        if (send_bc < 0) {
            printf("unable to send broadcast\n");
            close(listen_socket);
            return NULL;
        }
        for (int i = 0; i < MAX_ROOMS; i++) {
            memset(read_buffer, 0, MAX_MSG_SIZE);
            if (get_broadcast_msg(&listen_socket, read_buffer, DEFAULT_RECEIVE_SIZE, &sender_addr)) {
                //read room name, room description, number of players, ip
                room_t *for_display = calloc(1, sizeof(room_t));
                if (!read_room_info(read_buffer, for_display) ||
                    inet_ntop(AF_INET, &sender_addr.sin_addr, for_display->server_ip, (size_t)sizeof(for_display->server_ip)) == NULL) {
                    i--;
                    free(for_display);
                    printf("wrong format: %s\n", read_buffer);
                    continue;
                }
                room_infos[i] = for_display;
            } else {
                break;
            }
        }
        char *msg_buff = calloc(MAX_MSG_SIZE, 1);
        //send room info to dsiplay
        for (int i = 0; i < MAX_ROOMS; i++) {
            if (room_infos[i] != NULL) {
                strcat(msg_buff, (room_infos[i])->room_name);
                strcat(msg_buff, "|");
                strcat(msg_buff, (room_infos[i])->description);
                strcat(msg_buff, "|");
            } else {
                break;
            }
        }
        printf("send msg: %s\n", msg_buff);
        send_display_msg(msg_buff, MAX_MSG_SIZE);
        free(msg_buff);
        
        //receive from display to join room(by search or explicit ip), host room, refresh or exit lobby
        //all in string
        //join room: 0 which_room_in_the_array
        //host room: 1 name description
        //refresh: 2 
        //exit: 3
        memset(read_buffer, 0, MAX_MSG_SIZE);
        int room_numb = 0;
        char *name;
        char *description;
        room_t *new_room;
        receive_display_msg(read_buffer, MAX_MSG_SIZE);
        printf("msg received: %s\n", read_buffer);
        char *type = strtok(read_buffer, "|");
        if (strlen(type) != 1) {
            printf("incorrect msg\n");
            continue;
        }
        switch (type[0])
        {
        case '0':
            printf("case 0\n");
            room_numb = atoi(strtok(NULL, "|"));
            if (room_infos[room_numb] == NULL) {
                printf("incorrect msg\n");
                break;
            }
            printf("start connection on %s\n", room_infos[room_numb]->server_ip);
            if(!join_room(room_infos[room_numb]->server_ip, &stream_socket, display_socket)) {
                printf("connection failed\n");
                break;
            }
            printf("connected to host\n");
            goto exit_lobby;
        case '1':
            new_room = malloc(sizeof(room_t));
            name = strtok(NULL, "|");
            if (name == NULL) {
                break;
            }
            description = strtok(NULL, "|");
            if (description == NULL) {
                printf("incorrect msg\n");
                break;
            }
            create_room(new_room, name, description);
            if(!start_hosting(new_room, listen_socket, display_socket)) {
                printf("back to lobby\n");
                break;
            }             
            return new_room;
        case '2':
            break;
        case '3':
            goto exit_lobby;
        default:
            printf("incorrect message from display\n");
            break;
        }
        for (int i = 0; i < MAX_ROOMS; i++) {
            if (room_infos[i] != NULL) {
                free(room_infos[i]);
            } else {
                break;
            }
        }
    }
    exit_lobby:
    free(read_buffer);
    return NULL;
}

//=======================================================================================================

int main() {
    strcpy(device_ip, get_local_ip());
    if(!connect_to_stream_server("127.0.0.1", 59012, &display_socket)) {
        return EXIT_FAILURE;
    }
    printf("connected\n");
    enter_lobby(display_socket);
    return 0;
}
