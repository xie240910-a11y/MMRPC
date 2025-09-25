#include "test.pb.h"
#include <iostream>

using namespace fixbug;
using namespace std;

int main() {
    // LoginResponse rsp;
    // ResultCode *rc = rsp.mutable_result();
    // rc->set_errcode(1);
    // rc->set_errmsg("登录失败了");
    // rsp.set_success(true);

    GetFriendListResponse rsp;
    ResultCode *rc = rsp.mutable_result();
    rc->set_errcode(1);
    rc->set_errmsg("登录失败了");
    User *user1 = rsp.add_friend_list();
    user1->set_name("zhangsan");
    user1->set_age(20);
    user1->set_sex(User::MAN);

    User *user2 = rsp.add_friend_list();
    user2->set_name("li si");
    user2->set_age(22);
    user2->set_sex(User::MAN);

    cout << rsp.friend_list_size() << endl;

    return 0;
}

int main1(int argc, char const *argv[])
{
    LoginRequest req;
    req.set_name("zhang san");
    req.set_pwd("123456");
    std::string send_str;

    if (req.SerializeToString(&send_str)) {
        std::cout << send_str.c_str() << std::endl;
    }

    LoginRequest reqB;
    if (reqB.ParseFromString(send_str)) {
        std::cout << reqB.name() << ", " << reqB.pwd() << std::endl;
    } 

    return 0;
}

