## 测试backlog的影响

编译上面test_backlog.c程序：

    g++ -o test_backlog test_backlog.c

启动服务器程序:

    ./test_backlog 127.0.0.1 12345 5

当`telnet 127.0.0.1 12345`超过5次后，则服务器不会再接收.如下`netstat`清单

    ➜  ylx git:(master) ✗ netstat -nt|grep 12345
    tcp4       0      0  127.0.0.1.56698        127.0.0.1.12345        SYN_SENT   
    tcp4       0      0  127.0.0.1.56691        127.0.0.1.12345        SYN_SENT   
    tcp4       0      0  127.0.0.1.12345        127.0.0.1.56688        ESTABLISHED
    tcp4       0      0  127.0.0.1.56688        127.0.0.1.12345        ESTABLISHED
    tcp4       0      0  127.0.0.1.12345        127.0.0.1.56685        ESTABLISHED
    tcp4       0      0  127.0.0.1.56685        127.0.0.1.12345        ESTABLISHED
    tcp4       0      0  127.0.0.1.12345        127.0.0.1.56678        ESTABLISHED
    tcp4       0      0  127.0.0.1.56678        127.0.0.1.12345        ESTABLISHED
    tcp4       0      0  127.0.0.1.12345        127.0.0.1.56675        ESTABLISHED
    tcp4       0      0  127.0.0.1.56675        127.0.0.1.12345        ESTABLISHED
    tcp4       0      0  127.0.0.1.12345        127.0.0.1.56653        ESTABLISHED
    tcp4       0      0  127.0.0.1.56653        127.0.0.1.12345        ESTABLISHED


