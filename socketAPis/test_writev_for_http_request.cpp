/*
 * web服务器集中写测试
 *
 * 分散读和集中写,可以看做简化版recvmsg和sendmsg
 * 下面实现一个WEB服务器解析HTTP请求。
 * 如果目标文档存在且客户具有读取该文档的权限，那么它就需要发送一个HTTP应答来传输该文档。
 * 这个HTTP应答包含1个状态行、多个头部字段、1个空行和文档的内容。
 * 其中，前3部分的内容可能被Web服务器放置在一块内存中，
 * 而文档的内容则通常被读入到另外一块单独的内存中（通过read函数或mmap函数）。
 * 我们并不需要把这两部分内容拼接到一起再发送，而是可以使用writev函数将它们同时写出。
 *
 * Usage:
 * 编译并运行该程序：
 *      g++ -o main test_writev_for_http_request.cpp
 *      ./main 12345 myfile.txt
 *
 * 客户端:
 * 打开浏览器输 127.0.0.1:12345或curl 127.0.0.1:12345或telent 127.0.0.1 12345则都可获取到该文件
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024
/*定义两种HTTP状态码和状态信息*/
static const char* status_line[2] = {"200 OK", "500 Internal server error"};

int main(int argc, char **argv)
{
    if(argc < 3)
    {
        printf("Usage: port filename\n");
        return 1;
    }
    int port = atoi(argv[1]);
    const char* file_name = argv[2];        /*目标文件*/

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
    if(connfd < 0){
        printf("errno is:%d\n", errno);
    }else{
        /*关键部分来了.........*/
        char header_buf[BUFFER_SIZE];   /*用于保存HTTP应答的状态行、头部字段和一个空行的缓存区*/
        memset(header_buf, '\0', BUFFER_SIZE);

        char *file_buf;                 /*用于存放目标文件内容的应用程序缓存*/
        struct stat file_stat;          /*用于获取目标文件的属性，比如是否为目录，文件大小等*/
        bool valid=true;                /*记录目标文件是否是有效文件*/
        int len = 0;                    /*缓存区header_buf目前已经使用了多少字节的空间*/

        if(stat(file_name, &file_stat) < 0){    /*目标文件不存在*/
            valid = false;
        }else {
            if(S_ISDIR(file_stat.st_mode)){     /*目标文件是一个目录*/
                valid = false;
            }else if(file_stat.st_mode &S_IROTH){   /*当前用户有读取目标文件的权限*/
                /*
                 * 动态分配缓存区file_buf
                 * 并指定其大小为目标文件的大小file_stat.st_size + 1
                 * 然后将目标文件读入缓存区file_buf中
                 */
                int fd = open( file_name, O_RDONLY);
                file_buf = new char [file_stat.st_size + 1];
                memset( file_buf, '\0', file_stat.st_size + 1);

                if( read( fd, file_buf, file_stat.st_size) < 0 ) {
                    valid = false;
                }
            }else {
                valid = false;
            }
        }

        /*如果目标文件有效，则发送正常的HTTP应答*/
        if(valid) {
            /*
             * 将HTTP应答的状态行，"Content-Length"头部字段和一个空行依次加入header_buf中
             */
            ret = snprintf( header_buf, BUFFER_SIZE-1, "%s %s\r\n", "HTTP/1,1", status_line[0] );
            len += ret;
            ret = snprintf( header_buf + len, BUFFER_SIZE -1 -len,
                    "Content-Length:%lld\r\n", file_stat.st_size );
            len += ret;
            ret = snprintf( header_buf + len, BUFFER_SIZE -1 -len, "%s", "\r\n" );

            /*利用writev将header_buff和file_buff的内容一并写入*/
            struct iovec iv[2];
            iv[0].iov_base = header_buf;
            iv[0].iov_len = strlen(header_buf);
            iv[1].iov_base = file_buf;
            iv[1].iov_len = file_stat.st_size;
            ret = writev(connfd, iv, 2);
        } else {  /*如果目标文件无效则通知客户端服务器发送500异常*/
            ret = snprintf( header_buf, BUFFER_SIZE-1, "%s %s\r\n", "HTTP/1.1", status_line[1]);
            len += ret;
            ret = snprintf( header_buf + len, BUFFER_SIZE-1-len, "%s", "\r\n");
            send( connfd, header_buf, strlen(header_buf), 0 );
        }

        close(connfd);
        delete [] file_buf;

    }

    close(sock);
    return 0;
}
