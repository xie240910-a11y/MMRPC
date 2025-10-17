#include <iostream>
#include "mprpcapplication.h"
#include "friend.pb.h"

int main(int argc, char *argv[])
{
    // 整个程序启动以后，想要使用mprpc框架来享受rpc服务调用，一定要先调用框架的初始化函数（只初始化一次）
    MprpcApplication::Init(argc, argv);

    // 演示调用远程发布的rpc方法Login
    fixbug::FriendServiceRpc_Stub stub(new MprpChannel());
    // rpc方法的请求参数
    fixbug::GetFriendListsRequest request;
    request.set_userid(1000);

    // rpc方法的响应
    fixbug::GetFriendListsResponse response;
    MprpcController controller;

    // 发起rpc方法的调用 同步的rpc调用过程 MprpcChannel::CallMethod()
    stub.GetFriendsList(&controller, &request, &response, nullptr); // RpcChannel -> RpcChannel::callMethod集中来做所有rpc方法调用的参数序列化跟网络发生，调用装类
    if (controller.Failed())
    {
        std::cout << controller.ErrorText() << "\n";
    }
    else
    {
        // 一次rpc调用完成读响应结果
        if (response.result().errcode() == 0)
        {
            size_t friends = response.friends_size();
            for (size_t i = 0; i < friends; ++i)
            {
                std::cout << "index: " << (i + 1) << ": friend name = " << response.friends(i) << "\n";
            }
        }
        else
        {
            std::cout << "rpc GetFriendsList response error: " << response.result().errmsg() << "\n";
        }
    }

    return 0;
}
