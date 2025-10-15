#include <iostream>
#include "test.pb.h"

void test1()
{
    fixbug::LoginRequest req;
    req.set_name("zhangsan");
    req.set_pwd("123456");
    std::string send_str;

    // 对线数据序列化
    if (req.SerializeToString(&send_str))
    {
        std::cout << send_str.c_str() << std::endl;
    }

    fixbug::LoginRequest req2;
    if (req2.ParseFromString(send_str))
    {
        std::cout << req2.name() << std::endl;
        std::cout << req2.pwd() << std::endl;

    }
}

void test2()
{
    // LoginResponse rsp;
    // ResultCode *rc = rsp.mutable_result();
    // rc->set_errcode(0);
    // rc->set_errmsg("success");
    // rsp.set_success(true);

    fixbug::GetFriendListsResponse rsp;
    fixbug::ResultCode *rc = rsp.mutable_result();
    rc->set_errcode(0);
    rc->set_errmsg("success");

    fixbug::User *user1 = rsp.add_friend_list();
    user1->set_name("zhangsan");
    user1->set_age(20);
    user1->set_sex(fixbug::MAN);

    fixbug::User *user2 = rsp.add_friend_list();
    user2->set_name("xiaoqian");
    user2->set_age(18);
    user2->set_sex(fixbug::WOMAN);

    std::cout << rsp.friend_list_size() << std::endl;
    std::cout << rsp.friend_list(0).name() << std::endl;
    std::cout << rsp.friend_list(0).age() << std::endl;
    std::cout << rsp.friend_list(0).sex() << std::endl;

}

int main(int argc, char const *argv[])
{
    test2();
    return 0;
}