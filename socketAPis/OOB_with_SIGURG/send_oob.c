#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <unistd.h>

int my_connect(const char ip[], const char port[])
{

}

int main(int argc, char **argv)
{
    int sockfd;
    if(argc != 3)
    {
        perror("usage: %s <host> <port>");
        exit(1);
    }
    sockfd = my_connect(argv[1], argv[2]);

    /*发送普通*/
    write(sockfd, "123", 3);
    printf("发送3字节普通数据:123\n");
    /*sleep让套接字处于阻塞状态，使write和send的数据能够作为单个TCP分段发送对端*/
    sleep(1);

    /*发送带外数据*/
    send(sockfd, "4", 1, MSG_OOB);
    printf("发送1字节带外数据:4\n");
    sleep(1);

    write(sockfd, "56", 2);
    printf("发送2字节普通数据:56\n");
    sleep(1);

    send(sockfd, "7", 1, MSG_OOB);
    printf("发送1字节带外数据:7");
    sleep(1);

    write(sockfd, "89", 2);
    printf("发送2字节普通数据:89\n");
    sleep(1);
    exit(0);
}
