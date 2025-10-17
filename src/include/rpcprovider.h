#pragma once
#include "google/protobuf/service.h"
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>
#include <muduo/base/Timestamp.h>
#include <unordered_map>

// 框架提供的专门服务发布rpc服务的网络对象
class RpcProvider
{
private:
    // 组合了TcpLoop
    muduo::net::EventLoop m_eventLoop;
    struct ServiceInfo{
        google::protobuf::Service *m_service; // 服务对象
        std::unordered_map<std::string, const google::protobuf::MethodDescriptor*> m_methodMap;
    };
    std::unordered_map<std::string, ServiceInfo> m_serviceMap;
    // 新的socket连接回调
    void onConnection(const muduo::net::TcpConnectionPtr&);
    // 已建立连接读写事件回调
    void onMessage(const muduo::net::TcpConnectionPtr&, muduo::net::Buffer*, muduo::Timestamp);
    // Closure的回调操作，用于序列号rpc的响应跟网络发送
    void sendRpcResponse(muduo::net::TcpConnectionPtr, google::protobuf::Message*);
public:
    // 这里是框架提供给外部使用的，可以发布rpc方法
    void NotifyService(google::protobuf::Service *service);

    // 启动rpc服务节点，开始提供rpc远程网络调用服务
    void Run();
    RpcProvider() = default;
    ~RpcProvider() = default;

};