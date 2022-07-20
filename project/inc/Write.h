#include <arpa/inet.h>
#include <hiredis/hiredis.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#define N 100010

//添加头部（消息长度）、避免tcp粘包和分包问题
char Write_str[N];  //真正发送的msg
char my_str[N];
void init() {
    int msg_len = strlen(my_str);
    memset(Write_str, 0, 10);
    int idx = 0, x = msg_len;

    while (x) {
        Write_str[idx++] = (char)('0' + (x % 10));
        x /= 10;
    }
    int len = strlen(Write_str), i;
    for (i = len; i < 5; i++) {
        Write_str[i] = '0';
    }

    for (i = 0; i < 2; i++) {
        char tmp = Write_str[i];
        Write_str[i] = Write_str[4 - i];
        Write_str[4 - i] = tmp;
    }

    for (i = 5; i < msg_len + 5; i++) {
        Write_str[i] = my_str[i - 5];
    }
    Write_str[msg_len + 5] = '\0';
    return;
}

void Write(int client_sock) {  //加上5字节头部，表示msg长度，避免分包和粘包问题
    init();  //添加头部
    int len = write(client_sock, Write_str, strlen(Write_str));
    // printf("消息发送成功： %s\n", Write_str);
}