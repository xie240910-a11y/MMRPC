#include "zookeeperutil.h"
#include "mprpcapplication.h"
#include <iostream>

// 全局watcher观察器
void global_watcher(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx)
{
    if (type == ZOO_SESSION_EVENT) // 回调的消息类型是和会话相关的消息类型
    {
        if (state == ZOO_CONNECTED_STATE) // zkclient跟zkserver连接成功
        {
            sem_t *sem = (sem_t *)zoo_get_context(zh);
            sem_post(sem);
        }
    }
}

ZkClient::ZkClient() : m_zhandle(nullptr)
{}

ZkClient::~ZkClient()
{
    if (m_zhandle != nullptr)
    {
        zookeeper_close(m_zhandle); // 关闭句柄 释放资源
    }
}
// zkclient启动连接zkserver
void ZkClient::start()
{
    std::string host = MprpcApplication::getInstance().getConfig().Load("zookeeperip");
    std::string port = MprpcApplication::getInstance().getConfig().Load("zookeeperport");
    std::string connstr = host + ":" + port;
    
    /*
    zookeeper_mt: 多线程版本
    zookeeper的API客户端程序提供了三个线程
    API调用线程
    网络I/O线程 pthread_create poll
    watcher回调线程
    */
    m_zhandle = zookeeper_init(connstr.c_str(), global_watcher, 30000, nullptr, nullptr, 0);
    if (nullptr == m_zhandle)
    {
        std::cout <<"zookeeper_init error!\n";
        exit(EXIT_FAILURE);
    }
    sem_t sem;
    sem_init(&sem, 0, 0);
    zoo_set_context(m_zhandle, &sem);

    sem_wait(&sem);
    std::cout << "zookeeper_init success!\n";

}
// 在zkserver上根据据指定的path创建znode节点
void ZkClient::create(const char *path, const char *data, int datalen, int state)
{
    char path_buffer[128]{};
    int bufferlen = sizeof(path_buffer);
    int flag;
    
    //先判断path 表示znode节点是否存在，如果存在就不再重复创建了
    flag = zoo_exists(m_zhandle, path, 0, nullptr);
    if (ZNONODE == flag) // path的znode节点不存在
    {
        // 创建指定path的znode节点了
        flag = zoo_create(m_zhandle, path, data, datalen,
             &ZOO_OPEN_ACL_UNSAFE, state, path_buffer, bufferlen);
        if (flag == ZOK)
        {
            std::cout << "znode create success... path: " << path << "\n";
        }
        else
        {
            std::cout << "flag: " << flag << "\n";
            std::cout << "znode create error... path: " << path << "\n";
            exit(EXIT_FAILURE);
        }
    }

}
// 根据指定的znode节点路径 获取znode节点的值
std::string ZkClient::getData(const char *path)
{
    char buffer[64];  
    int bufferlen = sizeof(buffer);
    int flag = zoo_get(m_zhandle, path, 0, buffer, &bufferlen, nullptr);
    if (flag != ZOK)
    {
        std::cout << "get znode error... path: " << path << "\n";
        return "";
    }
    else
    {
        return buffer;
    }
}