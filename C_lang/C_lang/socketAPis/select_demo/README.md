I/O 多路复用之select

编译并运行select_demo程序

客户端：

    telnet 127.0.0.1 12345
    Trying 127.0.0.1...
    Connected to localhost.
    Escape character is '^]'.
    good
    select
    ^]
    telnet> quit
    Connection closed.

服务端：

    ./main 12345
    listen on port:12345
    get 6 bytes of normal data:good

    get 8 bytes of normal data:select

    recv read_fd failed.

