《Linux高性能服务器编程》第五章，代码清单5-5

如果监听队列中处于`ESTABLISHED`状态的连接对应的客户端出现网络异常（比如掉线），或者提前退出，那么服务器对这个连接执行的`accept`调用是否成功？

编写一个简单的服务器程序来测试之.

测试：
    ./conn_when_shutdown_test_accept 127.0.0.1 12345
    
    启动telnet客户端程序后，立即断开该客户端的网络连接（建立和断开连接的过程要在服务器启动后20秒内完成
    ➜  ylx git:(master) ✗ telnet 127.0.0.1 12345
    Trying 127.0.0.1...

结果发现accept调用能够正常返回，服务器输出如下：

    Connected with ip:127.0.0.1 and port:54718

重新执行上述过程，不过这次不断开客户端网络连接，而是在建立连接后立即退出客户端程序。这次accept调用同样正常返回.

可通过`netstat -an|grep 12345` 查看网络状态

**由此可见，accept只是从监听队列中取出连接，而不论连接处于何种状态（如上面的`ESTABLISHED`状态和`CLOSE_WAIT`状态），更不关心任何网络状况的变化。**


