#include <iostream>
#include "mprpcapplication.h"
#include "user.pb.h"
#include "mprpcchannel.h"

int main(int argc, char *argv[])
{
    // 整个程序启动以后，想要使用mprpc框架来享受rpc服务调用，一定要先调用框架的初始化函数（只初始化一次）
    MprpcApplication::Init(argc, argv);

    // 演示调用远程发布的rpc方法Login
    fixbug::UserServiceRpc_Stub stub(new MprpChannel());
    // rpc方法的请求参数
    fixbug::LoginRequest request;
    request.set_name("zhangsan");
    request.set_pwd("123");
    // rpc方法的响应
    fixbug::LoginResponse response;
    // 发起rpc方法的调用 同步的rpc调用过程 MprpcChannel::CallMethod()
    stub.Login(nullptr, &request, &response, nullptr); // RpcChannel -> RpcChannel::callMethod集中来做所有rpc方法调用的参数序列化跟网络发生，调用装类

    // 一次rpc调用完成读响应结果
    if (response.result().errcode() == 0)
    {
        std::cout << "rpc login response success: " << response.success() << "\n";
    }
    else
    {
        std::cout << "rpc login response error: " << response.result().errmsg() << "\n";
    }

    // rpc方法的请求参数
    fixbug::RegisterRequest register_req;
    register_req.set_id(2000);
    register_req.set_name("mprpc");
    register_req.set_pwd("666666");
    // rpc方法的响应
    fixbug::RegisterResponse register_rsp;

    // 同步方式发起请求
    stub.Register(nullptr, &register_req, &register_rsp, nullptr);
    if (register_rsp.result().errcode() == 0)
    {
        std::cout << "rpc register response success: " << register_rsp.success() << "\n";
    }
    else
    {
        std::cout << "rpc register response error: " << register_rsp.result().errmsg() << "\n";
    }
    return 0;
}
