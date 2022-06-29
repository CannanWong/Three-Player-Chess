#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#define MAX_NAME_SIZE 128
int main() {
    int bc_port = 41890;
    int bc_socket;

    char hostname[MAX_NAME_SIZE];
    if (gethostname(hostname, MAX_NAME_SIZE) != 0) {
        printf("failed to get hostname\n");
        exit(EXIT_FAILURE);
    }
    struct hostent *he;
    he = gethostbyname(hostname);
    struct in_addr a;
    bcopy(*he->h_addr_list++, (char *) &a, sizeof(a));
    printf("address: %s\n", inet_ntoa(a));

    char *msg_ip = inet_ntoa(a);

  
    if ((bc_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        printf("failed to conenct to socket\n");
        exit(EXIT_FAILURE);
    }
    int bc_permission = 1;
    int ret = setsockopt(bc_socket, SOL_SOCKET, SO_BROADCAST, &bc_permission, sizeof(bc_permission));
    if (ret) {
        printf("failed to set socket\n");
        close(bc_socket);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in bc_addr;
    memset(&bc_addr, 0, sizeof(bc_addr));
    bc_addr.sin_family = AF_INET;
    bc_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    bc_addr.sin_port = htons(bc_port);

    char *msg = "testing boardcast";
    for (int i = 0; i != 20; i++) {
        int ret2 = sendto(bc_socket, msg, strlen(msg), 0, (struct sockaddr*)&bc_addr, sizeof(bc_addr));
    
        if (ret2 < 0) {
            printf("unable to send\n");
            close(bc_socket);
            exit(EXIT_FAILURE);
        }
        sleep(1);
    }
    close(bc_socket);
    return 0;
}