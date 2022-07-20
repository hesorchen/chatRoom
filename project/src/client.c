#include <arpa/inet.h>
#include <hiredis/hiredis.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../inc/Read.h"
#include "../inc/Write.h"
/*
编译： gcc -o client client.c -L/usr/local/lib/ -lhiredis 
*/
void put_error(char* s) {
    printf("%s\n", s);
    exit(1);
}

void debug() {
    strcpy(Read_str, "R hesorchen 1234");
    exit(0);
}

int main(int argc, char** argv) {
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof server_addr);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));

    int client_sock = socket(PF_INET, SOCK_STREAM, 0);
    socklen_t server_addr_size = sizeof(server_addr);
    if (connect(client_sock, (struct sockaddr*)&server_addr,
                server_addr_size) == -1) {
        put_error("connect() error!");
    }
    Read(client_sock);
    printf("%s\n", Read_str);

    while (1) {
        printf("================================\n");
        gets(my_str);
        Write(client_sock);
        Read(client_sock);
        printf("%s\n", Read_str);
    }

    //关闭sockss
    close(client_sock);
    return 0;
}
