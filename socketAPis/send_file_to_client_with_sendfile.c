/*
 * 利用sendfile函数将服务器上的一个文件传送给客户端
 * sendfile函数在两个文件描述符之间之间传递数据，完全内核操作，
 * 避免了内核缓冲区和用户缓冲区的交互，效率更高。在<sys/sendfile.h>中
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
//#include <sys/sendfile.h>
/*
 * On OSX sendfile is defined in socket.h. So you need to include socket.h instead of sys/sendfile.h
 * The OS X Developer Library states you should include these:
 *
 *  #include <sys/types.h>
 *  #include <sys/socket.h>
 *  #include <sys/uio.h>
 */
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char **argv)
{
    if (argc <= 3)
    {
        printf("Usage: %s ip port filename\n", basename(argv[0]));
        return 1;
    }
    const char *ip = argv[1];
    int port = atoi(argv[2]);
    const char *file_name = argv[3];

    int filefd = open(file_name, O_RDONLY);
    assert( filefd > 0);
    struct stat stat_buf;
    fstat( filefd, &stat_buf);

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    inet_pton(AF_INET, ip, &address.sin_addr);

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    assert(sock >= 0);

    int ret = bind(sock, (struct sockaddr*)&address, sizeof(address));
    assert(ret!=-1);

    ret = listen(sock, 5);
    assert(ret!=-1);

    struct sockaddr_in client;
    socklen_t client_addrlength = sizeof(client);
    int connfd = accept(sock, (struct sockaddr*)&client, &client_addrlength);
    if(connfd <0){
        printf("errno:is %d\n", errno);
    } else {
        /*关键部分来了*/

        sendfile(connfd, filefd, NULL, stat_buf.st_size);
        close(connfd);
    }
    return 0;
}
