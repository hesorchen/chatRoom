#include <arpa/inet.h>
#include <hiredis/hiredis.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#include "../inc/Read.h"
#include "../inc/Write.h"

/*
gcc -o server server.c -L/usr/local/lib/ -lhiredis
*/

#define EPOLL_SIZE 100
redisContext *conn;
redisReply *reply;
void put_error(char *s) {
    printf("%s\n", s);
    exit(1);
}

int logged[N];  //记录客户端是否已登录

void out() { printf("============ fuck ===========\n"); }

void addToHtml(char str[]) {
    char strEcho[100] = "echo \"";
    char *tmp1 = strcat(strEcho, str);
    char *tmp2 = strcat(tmp1, "\" >> /var/www/html/index.html");
    system((const char *)tmp2);
}
//回复注册/登陆消息
void func_log(int sock) {
    char opt[100];
    char count[100];
    char password[100];
    memset(opt, 0, sizeof opt);
    memset(count, 0, sizeof count);
    memset(password, 0, sizeof password);
    int n = strlen(Read_str);
    int id = 0, f = 1, cnt = 0, i;
    for (i = 0; i < n; i++) {
        if (Read_str[i] == ' ') {
            f = 1;
            continue;
        }
        if (f) f = 0, id = 0, cnt++;
        if (cnt == 1)
            opt[id++] = Read_str[i];
        else if (cnt == 2)
            count[id++] = Read_str[i];
        else if (cnt == 3)
            password[id++] = Read_str[i];
    }
    if (strlen(password) == 0 || !(!strcmp(opt, "L") + !strcmp(opt, "R"))) {
        strcpy(my_str, "注册/登陆消息格式错误，请重新发送！\0");
        Write(sock);
        return;
    }
    //注册
    if (strcmp(opt, "R") == 0) {
        reply = redisCommand(conn, "Hexists counts_password %s", count);
        if (reply->integer == 1) {  //账号存在
            strcpy(my_str, "该账号已经存在！\0");
        } else {  //账号不存在
            reply = redisCommand(conn, "Hset counts_password %s %s", count,
                                 password);
            strcpy(my_str, "账号创建成功！请登陆！\0");
            addToHtml(count);
        }
    } else if (strcmp(opt, "L") == 0) {  //登陆
        reply = redisCommand(conn, "Hexists counts_password %s", count);
        if (reply->integer == 1) {  //账号存在
            reply = redisCommand(conn, "Hget counts_password %s", count);
            if (strcmp(reply->str, password) == 0) {  //密码正确
                logged[sock] = 1;  //标记该sock已经登陆
                reply =
                    redisCommand(conn, "Hset sock_count %d %s", sock, count);
                strcpy(my_str, "登陆成功！\0");
            } else {  //密码错误
                strcpy(my_str, "账号或密码不正确！\0");
            }
        } else {  //账号不存在
            strcpy(my_str, "账号或密码不正确！\0");
        }
    }
    Write(sock);
}

void func_write(int sock) {  //将客户端发送的消息写入redis
    char *name;
    reply = redisCommand(conn, "Hget sock_count %d", sock);
    name = reply->str;
    time_t tmpcal_ptr;
    struct tm *tmp_ptr = NULL;
    time(&tmpcal_ptr);
    tmp_ptr = localtime(&tmpcal_ptr);
    sprintf(my_str, "%d.%d.%d %d:%d:%d     %s\n%s", (1900 + tmp_ptr->tm_year),
            (1 + tmp_ptr->tm_mon), tmp_ptr->tm_mday, tmp_ptr->tm_hour,
            tmp_ptr->tm_min, tmp_ptr->tm_sec, name, Read_str);
    printf("%s\n", my_str);
    printf("============================================\n");
    reply = redisCommand(conn, "RPUSH message_record %s", my_str);
    strcpy(my_str, "消息发送成功！\0");
    Write(sock);
}

void func_message_record() {
    reply = redisCommand(conn, "Llen message_record");
    int len = reply->integer;
    reply = redisCommand(conn, "Lrange message_record 0 %d", len);
    int i;
    for (i = 0; i < reply->elements; i++) {
        printf("---------------------------------------\n");
        printf("%s\n", reply->element[i]->str);
    }
}

void debug() {
    func_message_record();
    exit(0);
}
void connect_to_redis() {  //调用API连接Redis
    conn = redisConnect("127.0.0.1", 6379);
    char password[] = "SF@chenyan";
    redisReply *reply = redisCommand(conn, "auth %s", password);
    freeReplyObject(reply);
    if (conn != NULL && conn->err) {
        printf("connection error: %s\n", conn->errstr);
        exit(0);
    }
}

int main(int argc, char **argv) {
    connect_to_redis();
    if (argc <= 1) {
        put_error("Need to use port number");
    }
    int server_sock = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    memset(&server_addr, 0,
           sizeof server_addr);  //初始化结构体，主要是初始化addr.zero
    server_addr.sin_port =
        htons(atoi(argv[1]));  //将传入的端口转化为int型并转化为网络字节序
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);  //自动获取服务端的ip地址

    if (bind(server_sock, (struct sockaddr *)(&server_addr),
             sizeof(server_addr)) == -1) {
        put_error("bind() error!\n");
    }

    if (listen(server_sock, 10) == -1) {
        put_error("listen() error!\n");
    }
    struct epoll_event ep_events[1000];
    struct epoll_event tmp_event;
    int epfd, event_cnt;
    epfd = epoll_create(1);

    tmp_event.data.fd = server_sock;
    tmp_event.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_ADD, server_sock, &tmp_event);
    int client_sock, i;
    while (1) {
        // printf("====================================================================\n");
        event_cnt = epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);
        for (i = 0; i < event_cnt; i++) {
            if (ep_events[i].data.fd == server_sock) {  //有客户端请求连接
                socklen_t client_addr_size;
                client_addr_size = sizeof(client_addr);
                if ((client_sock = accept(
                         server_sock, (struct sockaddr *)(&client_addr),
                         &client_addr_size)) == -1)  //接受连接，创建client_sock
                    put_error("accept() error!\n");
                printf("客户端 %d 连接成功 \n", client_sock);
                strcpy(
                    my_str,
                    "以[选项 账号 密码] 的消息格式，选择注册或登陆账号，\
                       R为注册，L为登陆，例如 ：\nR chen abc123\nL hesor 123abc\0");
                Write(client_sock);
                logged[client_sock] = 0;
                tmp_event.data.fd = client_sock;
                tmp_event.events = EPOLLIN;
                epoll_ctl(epfd, EPOLL_CTL_ADD, client_sock, &tmp_event);
            } else {  //有客户端发送数据
                int sock = ep_events[i].data.fd;
                int len = Read(sock);
                if (len == 0) {
                    close(sock);
                }
                if (!logged[sock]) {
                    func_log(sock);  //登陆
                } else {
                    func_write(sock);
                }
            }
        }
    }

    //关闭sockss
    close(server_sock);
    close(client_sock);
    return 0;
}
