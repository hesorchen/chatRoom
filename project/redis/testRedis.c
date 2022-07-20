#include <hiredis/hiredis.h>
#include <stdio.h>

int main() {
    redisContext* conn = redisConnect("127.0.0.1", 6379);
    if (conn != NULL && conn->err) {
        printf("connection error: %s\n", conn->errstr);
        return 0;
    } else {
        printf("connection successful\n");
    }
    char password[] = "SF@chenyan";
    redisReply* reply = redisCommand(conn, "auth %s", password);
    printf("%s\n", reply->str);
    freeReplyObject(reply);

    reply = (redisReply*)redisCommand(conn, "SET foo fuck");
    printf("%s\n", reply->str);
    freeReplyObject(reply);
    puts("2222");

    int i;
    reply = redisCommand(conn, "GET foo");
    for (i = 0; i < reply->elements; i++) {
        printf("%s\n", reply->element[i]->str);
    }
    freeReplyObject(reply);

    redisFree(conn);
    return 0;
}
/*
gcc -o  testRedis testRedis.c -L/usr/local/lib/ -lhiredis
*/
