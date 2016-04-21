/*使用select 同时接收普通数据和带外数据*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char **argv)
{
    if(argc <= 1)
    {
        printf("Usage: %s port\n", argv[0]);
        return 1;
    }
    int port = atoi(argv[1]);
    printf("listen on port:%d\n", port);

    /*服务端地址*/
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    /*创建套接字*/
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    assert(server_fd != -1);

    /*绑定套接字*/
    int ret = bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    assert(ret != -1);

    /*监听*/
    ret = listen(server_fd, 5);
    assert(ret != -1);

    /*客户端地址*/
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    socklen_t client_addr_len = sizeof(client_addr);

    /*接收客户端连接*/
    int talk_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_addr_len);
    assert(talk_fd != -1);

    /*接收信息缓冲区*/
    char buff[1024];
    memset(buff, 0, sizeof(buff));

    fd_set read_fd;         //关注的可读文件描述符集合
    fd_set exception_fd;    //关注的异常文件描述符集合
    FD_ZERO(&read_fd);      //清空可读文件描述符的所有位
    FD_ZERO(&exception_fd); //清空异常文件描述符的所有位

    while(1)
    {
        /*清空信息缓冲区*/
        memset(buff, 0, sizeof(buff));
        /*
         * 每次调用select之前需要重新设置文件描述符集合，
         * 因为事件发生之后文件描述符集合被内核修改
         * */
        FD_SET(talk_fd, &read_fd);  /*将客户端、服务端的socket描述符加入可读描述符集合*/
        FD_SET(talk_fd, &exception_fd); /*将客户端、服务端的socket描述符加入异常描述符集合*/

        /*阻塞在select调用，直到可读、异常描述符集合发生状态变化*/
        ret = select(talk_fd + 1, &read_fd, NULL, &exception_fd, NULL);
        assert(ret != -1);

        /*对于可读事件，采用普通的recv函数读取数据*/
        if(FD_ISSET(talk_fd, &read_fd))
        {
            ret = recv(talk_fd, buff, sizeof(buff) - 1, 0);
            if (ret <= 0){
                printf("recv read_fd failed.\n");
                break;
            }
            //输出接收到的普通数据
            printf("get %d bytes of normal data:%s\n", ret, buff);
        }

        /*对于异常事件，采用带MSG_OOB标志的recv函数接收数据*/
        if(FD_ISSET(talk_fd, &exception_fd))
        {
            ret = recv(talk_fd, buff, sizeof(buff) - 1, MSG_OOB);
            if(ret <= 0)
            {
                printf("recv exception_fd failed.\n");
                break;
            }
            //输出接收到的异常数据
            printf("get %d bytes of oob data:%s\n", ret, buff);
        }
    }
    close(talk_fd);
    close(server_fd);
    return 0;
}
