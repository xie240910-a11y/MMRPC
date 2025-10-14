#include <iostream>
#include <string>
#include "user.pb.h"
#include "mprpcapplication.h"
#include "rpcprovider.h"
using std::string;
using std::cout;
using std::endl;
using namespace user;

class UserService:public UserServiceRpc
{
public:
    bool Login(string name, string pwd)
    {
        cout <<  "doing local service: Login" << endl;
        cout <<  "name: " << name << ", pwd: " << pwd << endl;
        return true;
    }

    void Login(::google::protobuf::RpcController* controller,
                       const ::user::LoginRequest* request,
                       ::user::LoginResponse* response,
                       ::google::protobuf::Closure* done)
    {
        string name = request->name();
        string pwd = request->pwd();
        
        // 做本地业务
        bool login_result = Login(name, pwd);

        // 把响应写入
        user::ResultCode *code = response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("");
        response->set_success(login_result);
        
        // 执行回调·
        done->Run();
    }

};

int main(int argc, char const *argv[])
{
    MprpcApplication::Init(argc, argv);
    RpcProvider provider;
    provider.NotifyService(new UserService());
    provider.Run();
    return 0;
}
