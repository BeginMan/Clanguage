#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[])
{
    /*参数判断*/
    if(argc <= 3)
    {
        printf("Usage: %s ip port recv_buffer\n", basename(argv[0]));
        return 1;
    }

    const char *ip = argv[1];
    int port = atoi(argv[2]);

    /*创建IPv4 socket地址*/
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    inet_pton(AF_INET, ip, &address.sin_addr);

    /*创建socket*/
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    assert(sock != -1);

    /*设置socket选项SO_RCVBUF接收缓冲区大小*/

    int buffer = atoi(argv[3]);
    int len = sizeof(buffer);
    setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &buffer, len);
    getsockopt(sock, SOL_SOCKET, SO_RCVBUF, &buffer, (socklen_t *)&len);
    printf("the receive buffer size :%d\n", buffer);

    /*绑定socket(命名socket)*/
    int ret = bind(sock, (struct sockaddr*)&address, sizeof(address));
    assert(ret != -1);

    /*监听socket*/
    ret = listen(sock, 5);
    assert(ret != -1);

    /*接收socket*/
    //设置接收客户端socket地址
    struct sockaddr_in client;
    socklen_t client_addrlength = sizeof(client);
    int connfd = accept(sock, (struct sockaddr*)&client, &client_addrlength);
    if(connfd <0 )
    {
        printf("errno is:%d\n", errno);
    }
    else
    {
        /*接收数据，关闭连接socket*/
        char buf[BUFFER_SIZE];
        memset(buf, '\0', BUFFER_SIZE);
        while( recv(connfd, buf, BUFFER_SIZE-1, 0) > 0) {}
        close(connfd);
    }
    close(sock);
    return 0;
}

