#include <stdlib.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define MAXLINE     10
#define OPEN_MAX    100
#define LISTENQ     20
#define SERV_PORT   5555
#define INFTIM      1000

/*
 * 设置非阻塞socket 
 */
void setnonblocking(int sock)
{
    int opts;
    opts = fcntl(sock, F_GETFL);
    if(opts < 0)
    {
        perror("fcntl(sock, F_GETFL)");
        exit(1);
    }
    opts = opts|O_NONBLOCK;
    if(fcntl(sock, F_SETFL, opts) < 0)
    {
        perror("fcntl(sock, SETFL, opts)");
        exit(1);
    }
}

int main(int argc, char **argv)
{
    int i, 
        maxi,           /*设定epoll可管理的最大socket描述符数目*/
        listenfd,       /*监听socket*/
        connfd,         /*连接socket*/
        sockfd,         /*服务器socket*/
        epfd,           /*epoll专用的文件描述符*/
        nfds;           /*epoll_wait轮训函数返回值，返回发生事件数*/
    ssize_t n;
    char line[MAXLINE];
    socklen_t client;
    struct epoll_event ev,  /*声明epoll_event结构体的变量, ev用于注册事件*/ 
            events[20];     /*events数组用于回传要处理的事件*/
    
    /*
    * 生成用于处理accept的epoll专用的文件描述符
    * 指定生成描述符的最大范围为256
    * */
    epfd = epoll_create(256);

    struct sockaddr_in clientaddr;
    struct sockaddr_in serveraddr;
    
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    setnonblocking(listenfd);   /*把用于监听的socket设置为非阻塞方式*/
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    char *local_addr="192.168.1.120";
    inet_aton(local_addr, &(serveraddr.sin_addr));
    serveraddr.sin_port = htons(SERV_PORT);
    
    bind(listenfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    listen(listenfd, LISTENQ);    
    
    ev.data.fd = listenfd;      /*设置与要处理的事件相关的文件描述符*/
    ev.events = EPOLLIN|EPOLLET;/*设置要处理的事件类型*/
    /*
    * 注册epoll事件
    * */
    epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);

    maxi = 0;
    for(;;)
    {
        nfds = epoll_wait(epfd, events, 20, 500);   /*等待epoll事件发生*/
        for(i=0; i<nfds; ++i)
        {
            /*处理所发生的所有事件*/
            if(events[i].data.fd == listenfd)  /*监听事件*/
            {
                connfd = accept(listenfd, (struct sockaddr*)&clientaddr, &client);
                if(connfd < 0)
                {
                    perror("connfd<0");
                    exit(1);
                }
                setnonblocking(connfd);     /*把客户端的socket设置成非阻塞方式*/
                char *st = inet_ntoa(clientaddr.sin_addr);
                printf("connect from:%s\n", st);
                ev.data.fd = connfd;        /*设置用于读操作的文件描述符*/
                ev.events=EPOLLIN|EPOLLET;  /*设置用于注册的读操作事件*/
                epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev);    /*注册ev事件*/       
            }
            else if (events[i].events & EPOLLIN) 
            {
                /*读事件*/
                if ((sockfd = events[i].data.fd) < 0) continue;
                if ((n = read(sockfd, line, MAXLINE)) < 0)
                {
                    if(errno == ECONNRESET)
                    {
                        close(sockfd);
                        events[i].data.fd = -1;
                    }
                    else
                    {
                        printf("readline error");
                    }
                } 
                else if (n == 0)
                {
                    close(sockfd);
                    events[i].data.fd = -1;
                }
                ev.data.fd = sockfd;            /*设置用于写操作的文件描述符*/
                ev.events = EPOLLOUT|EPOLLET;   /*设置用于注册的写操作事件*/
                epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev);    /*修改sockfd上要处理的事件为EPOLLOUT*/
            }
            else if(events[i].events & EPOLLOUT)
            {
                /*写事件*/
                sockfd = events[i].data.fd;
                write(sockfd, line, n);
                ev.data.fd = sockfd;        /*设置用于读操作的文件描述符*/
                ev.events = EPOLLIN|EPOLLET;/*设置用于注册的读操作事件*/
                epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev); /*修改sockfd上要处理的事件为EPOLLIN*/
            }   
        }
    }   
   return 0; 
}

