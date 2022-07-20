#include <arpa/inet.h>
#include <hiredis/hiredis.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#define N 100010

//读取头部（消息长度）、避免tcp粘包和分包问题
char Read_str[N];
int Read(int client_sock) {
    read(client_sock, Read_str, 5);
    Read_str[5] = '\0';
    int len = atoi(Read_str);
    int cnt = 0;
    while (cnt < len) {
        int lenth = read(client_sock, Read_str + cnt, len - cnt);
        if (lenth == 0) {
            break;
        }
        cnt += lenth;
    }
    Read_str[cnt] = '\0';
    // printf("收到消息： %s\n", Read_str);
    return cnt;
}
