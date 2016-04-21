//
//  main.c
//  C_lang
//
//  Created by 方朋 on 16/4/21.
//  Copyright © 2016年 方朋. All rights reserved.
//

#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>     // ???
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>      //???
#include <string.h>

#define BUFFER_SIZE 1023

int setnonblocking(int fd);
int unblock_connection(const char *, int, int);

int main(int argc, const char * argv[]) {
    if (argc <= 2) {
        fprintf(stderr, "Usage: %s ip_address port.\n", argv[0]);
        return 1;
    }
    const char *ip = argv[1];
    int port = atoi(argv[2]);
    int sockfd = unblock_connection(ip, port, 10);
    if (sockfd < 0) return 1;
    close(sockfd);
    return 0;
}
