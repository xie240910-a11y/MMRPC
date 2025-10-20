#include "mprpcapplication.h"
#include "rpcprovider.h"
#include "rpcheader.pb.h"
#include "zookeeperutil.h"
#include <string>
#include <muduo/net/InetAddress.h>
#include <google/protobuf/descriptor.h>
/*
service_name => service描述
                service记录服务对象
                method方法描述
*/
void RpcProvider::NotifyService(google::protobuf::Service *service)
{
    ServiceInfo service_info;
    service_info.m_service = service;

    // 获取服务对象的描述信息
    const google::protobuf::ServiceDescriptor *pserviceDesc = service->GetDescriptor();
    // 获取服务的名称
    std::string service_name = pserviceDesc->name();
    std::cout << "service name: " << service_name << std::endl;
    // 获取服务对象service的方法的数量
    int methodCnt = pserviceDesc->method_count();
    for (size_t i = 0; i < methodCnt; ++i)
    {
        // 获取了服务对象指定下标的服务方法的具体描述
        const google::protobuf::MethodDescriptor* pmethodDesc = pserviceDesc->method(i);
        std::string method_name = pmethodDesc->name();
        std::cout << "method name: " << method_name << std::endl;
        service_info.m_methodMap.insert({method_name, pmethodDesc});
    }
    m_serviceMap.insert({service_name, service_info});
}

// 启动rpc服务节点，开始提供rpc远程网络调用服务
void RpcProvider::Run()
{
    std::string ip = MprpcApplication::getInstance().getConfig().Load("rpcserverip");
    uint16_t port = atoi(MprpcApplication::getInstance().getConfig().Load("rpcserverport").c_str());
    muduo::net::InetAddress address(ip, port);
    // 创建tcpServer对象
    static muduo::net::TcpServer server(&m_eventLoop, address, "rpcProvider");
    // 绑定连接回调跟消息读写回调
    server.setConnectionCallback(std::bind(&RpcProvider::onConnection, this, std::placeholders::_1));

    server.setMessageCallback(std::bind(&RpcProvider::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    // 设置muduo库的线程数量
    server.setThreadNum(4);

    // 把当前rpc节点上要发布的服务全部注册到zk上面，让rpc client 可以从zk上发现服务
    ZkClient zkCli;
    zkCli.start();
    // service_name为永久性节点， method_name为临时性节点
    // session timeout 30s zkclient 网络I/O线程 1/3 * timeout 时间发送ping消息
    for (auto &sp : m_serviceMap)
    {
        // /service_name
        std::string service_path = "/" + sp.first;
        zkCli.create(service_path.c_str(), nullptr, 0);
        for (auto &mp : sp.second.m_methodMap)
        {
            // /service_name /method_name
            std::string method_path = service_path + "/" + mp.first;
            char method_path_data[128]{};
            sprintf(method_path_data, "%s:%d", ip.c_str(), port);
            // 表示ZOO_EPHEMERAL是znode是一个临时性节点
            zkCli.create(method_path.c_str(), method_path_data, strlen(method_path_data), ZOO_EPHEMERAL);
        }
        
    }
    
    std::cout << "RpcProvider start service at ip: " << ip << ", port: " << port << std::endl;;
    server.start();
    m_eventLoop.loop();
}

void RpcProvider::onConnection(const muduo::net::TcpConnectionPtr& conn)
{
    if (!conn->connected())
    {
        std::cout << "connection closed by peer" << std::endl;
        return;
    }
}

/**
 * 在框架内部，RpcProvider跟RpcConsume协商好之间通信用的protobuf数据类型
 * service_name, method_name, args   定义proto的message类型，进行数据的序列号和反序列化
 * header_size(字节) + header_str + args_str
 */
void RpcProvider::onMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buff, muduo::Timestamp stamp)
{
    // 网络上接收的远程rpc调用请求的字符流
    std::string recv_buf = buff->retrieveAllAsString();
    
    // 从字符流中读取前4个字节的内容
    uint32_t header_size = 0;
    recv_buf.copy((char*)&header_size, 4, 0);
    header_size = ntohl(header_size); // ✅ 转换为主机字节序
    if (recv_buf.size() < 4 + header_size)
    {
        std::cout << "Error: recv_buf size(" << recv_buf.size()
                << ") < 4 + header_size(" << header_size << ")\n";
        return;
    }

    // 根据header_size读取数据头的原始数据字符流，反序列化数据，得到rpc请求的详细信息
    std::string rpc_header_str = recv_buf.substr(4, header_size);
    mprpc::RpcHeader rpcHeader;
    std::string service_name;
    std::string method_name;
    uint32_t args_size;
    std::string args_str;
    if (rpcHeader.ParseFromString(rpc_header_str))
    {
        // 反序列化成功
        service_name = rpcHeader.service_name();
        method_name = rpcHeader.method_name();
        args_size = rpcHeader.args_size();
    } 
    else
    {
        std::cout << "rpc_header_str: " << rpc_header_str << " parse error...\n";
    }
    args_str = recv_buf.substr(4 + header_size, args_size);
    std::cout << "======================================\n";
    std::cout << "rpc_header_str: " << rpc_header_str << "\n";
    std::cout << "header_size: " << header_size << "\n";
    std::cout << "service_name: " << service_name << "\n";
    std::cout << "method_name: " << method_name << "\n";
    std::cout << "args_size: " << args_size << "\n";
    std::cout << "args_str: " << args_str << "\n";
    std::cout << "======================================\n";

    auto it = m_serviceMap.find(service_name);
    if (it == m_serviceMap.end())
    {
        std::cout << service_name << " is not exists...\n";
        return;
    }

    auto mit = it->second.m_methodMap.find(method_name);
    if (mit == it->second.m_methodMap.end())
    {
        std::cout << service_name << ":" <<method_name << " is not exists...\n";
        return;
    }
    google::protobuf::Service * service = it->second.m_service; // 获取service对象
    const google::protobuf::MethodDescriptor *method = mit->second; // 获取method对象， Login
    
    google::protobuf::Message *request = service->GetRequestPrototype(method).New();
    if (!request->ParseFromString(args_str))
    {
        std::cout << "request parse error, content: " << args_str << "\n";
        return;
    }

    // ✅ 使用 Response 原型
    google::protobuf::Message *response = service->GetResponsePrototype(method).New();

    // ✅ 创建回调函数
    google::protobuf::Closure *done = 
        google::protobuf::NewCallback<RpcProvider,
                                        muduo::net::TcpConnectionPtr,
                                        google::protobuf::Message*>
                                        (this,&RpcProvider::sendRpcResponse,conn, response);

    // ✅ 调用服务方法
    service->CallMethod(method, nullptr, request, response, done);
}

void RpcProvider::sendRpcResponse(muduo::net::TcpConnectionPtr conn, google::protobuf::Message* response)
{
    std::string response_str;
    if (response->SerializeToString(&response_str))
    {
        // 序列化后，通过网路把rpc方法执行的结果发送给rpc调用方
        conn->send(response_str);
    }
    else
    {
        std::cout << "serialize response_str error!\n"; 
    }
    conn->shutdown(); // 模拟http短链接服务主动断开

}