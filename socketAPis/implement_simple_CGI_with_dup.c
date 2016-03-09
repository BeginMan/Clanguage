/*
 * 使用dup复制文件描述符实现简单的CGI服务器
 * Usage:
 * 1. 运行该程序
 * 2. $ telnet 127.0.0.1 12345
 *      Trying 127.0.0.1...
 *      Connected to localhost.
 *      Escape character is '^]'.
 *      hello,world
 *      Connection closed by foreign host.
 *
 */
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char **argv)
{
    int port = 12345;

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = htonl(INADDR_ANY);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    assert(sock != -1);

    int ret = bind(sock, (struct sockaddr*)&address, sizeof(address));
    assert(ret != -1);

    ret = listen(sock, 5);
    assert(ret != -1);

    struct sockaddr_in client;
    socklen_t client_addrlength = sizeof(client);
    int connfd = accept(sock, (struct sockaddr*)&client, &client_addrlength);
    if (connfd < 0) {
        printf("errno is:%d\n", errno);
    }else {
        /*关键部分来了*/
        close(STDOUT_FILENO);       /*先关闭标准输出文件描述符STDOUT_FILENO(值为1)*/
        dup(connfd);                /*然后复制socket文件描述符connfd,
                                        因为dup总是返回系统中最小的可用文件描述符
                                        所以它的返回值实际上是1
                                        即之前关闭的标准输出文件描述符的值*/
        printf("hello,world\n");    /*这样一来服务器printf输出会直接发送到与客户连接对应的socket上*/
        close(connfd);
    }
    close(sock);
    return 0;
}
