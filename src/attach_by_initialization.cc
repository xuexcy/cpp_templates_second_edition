/**
在不改变main函数的情况下(其他人/其他框架负责编写main逻辑)，向框架内添加一些初始化逻辑。

比如我写了一个用于数据采集的消息队列 class MessageQueue 库，用户可以通过
SEND_MSG("score", 1);
SEND_MSG("name", "bob");
等方式来采集数据。
在使用 MSG_SEND 前可能要做一些初始化操作，比如确定发送的地址，本地缓存大小等。
方法一: class MessageQueue 提供初始化接口，由编写 main 的开发主动调用, 比如
int main() {
    MessageQueue::Init();
    xxx
    xxx
}
方法二: 如果没有办法要求 main 的开发人员添加初始化操作，那么可以在 message_queue.cpp 中，
添加一个全局变量，这样全局变量会在 main 开始前进行初始化操作

注意: Objects in a namespace are created before any function/variable in that namespace is accessed
在没有正儿八经使用这个 global 时，这个全局变量可能不会被创造，比如注释掉下面代码中的 start.Start()，那么整个程序就
没有使用 MessageQueue，那么 global_msg_queue 也就不会在 main 执行前被构造
另外，多个全局变量之间的构造和析构顺序是不确定的，如果全局变量之间有构造依赖关系，那么结果可能不符合用户预期


*/

#include <cassert>
#include <print>

#include "attach_by_initialization_deps/server.h"
#include "utils/main_decorator.h"

int main() {
    utils::MainDecorator::Access();
    std::println("Start server");
    Server server;
    assert(true == server.Start() && "Start server failed\n");
    std::println("Quit server");
    return 0;
}
