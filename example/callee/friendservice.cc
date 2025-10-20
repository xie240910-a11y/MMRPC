#include <iostream>
#include <vector>
#include <string>
#include "friend.pb.h"
#include "mprpcapplication.h"
#include "rpcprovider.h"
#include "logger.h"

using std::string;
using std::cout;
using std::endl;

class FriendService : public fixbug::FriendServiceRpc
{
public:
    std::vector<string> GetFriendsList(uint32_t userid)
    {
        cout << "do GetFriendsList service!\n";
        std::vector<string> vec;
        vec.push_back("gaoyang");
        vec.push_back("liuhong");
        vec.push_back("wangshuo");
        return vec;
    }
    // 重写基类方法
    void GetFriendsList(::google::protobuf::RpcController* controller,
                       const ::fixbug::GetFriendListsRequest* request,
                       ::fixbug::GetFriendListsResponse* response,
                       ::google::protobuf::Closure* done)
    {
        uint32_t userid = request->userid();
        std::vector<string>  friendsList = GetFriendsList(userid);
        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("");
        for (string &name : friendsList)
        {
            string * p = response->add_friends();
            *p = name;
        }
        done->Run();
    }
};

int main(int argc, char *argv[])
{
    LOG_ERR("init friend service..");
    // 调用框架初始化操作
    MprpcApplication::Init(argc, argv);

    // 把UserService对象发布到rpc节点上
    RpcProvider provider;
    provider.NotifyService(new FriendService());

    // 启动一个prc服务发布节点
    provider.Run();
    return 0;
}
