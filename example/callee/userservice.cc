#include <iostream>
#include "user.pb.h"
#include "mprpcapplication.h"
#include "rpcprovider.h"

using std::string;
using std::cout;
using std::endl;

/*
UserService原来是一个本地服务，提供两个进程内的本地方法 Login GetFriendLists
*/

class UserService : public fixbug::UserServiceRpc // rpc服务提供者
{
public:
    bool Login(string name, string pwd)
    {
        cout << "doing Local Login FUNC()\n"; 
        cout << "name: " << name << ", pwd: " << pwd << endl; 
        return true;
    }

    /*
    重写基类的UserServiceRpc虚函数，下面都是框架直接调用
    1. caller ===> Login(LoginRequest) ==> moduo => callee
    2. callee ===> Login(LoginRequest) ==> 交到下面重写的这个Login方法上
    */
    void Login(::google::protobuf::RpcController* controller,
                       const ::fixbug::LoginRequest* request,
                       ::fixbug::LoginResponse* response,
                       ::google::protobuf::Closure* done)
    {
        // 框架给业务上报了请求参数LoginRequest，应用获取响应的本地业务
        string name = request->name();
        string pwd = request->pwd();
        
        bool login_result = Login(name, pwd); // 做本地业务

        // 把响应写入
        fixbug::ResultCode *code = response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("success");
        response->set_success(login_result);
        
        // 执行回调
        done->Run();
    }
};

int main(int argc, char *argv[])
{
    // 调用框架初始化操作
    MprpcApplication::Init(argc, argv);

    // 把UserService对象发布到rpc节点上
    RpcProvider provider;
    provider.NotifyService(new UserService());

    // 启动一个prc服务发布节点
    provider.Run();
    return 0;
}
