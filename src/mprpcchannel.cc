#include "mprpcchannel.h"
#include "mprpcapplication.h"
#include "mprpccontroller.h"
#include "zookeeperutil.h"
#include <string>
#include "rpcheader.pb.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

using std::string;

/**
 * header_size + service_name method_name args_size + args
 */
void MprpChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                          google::protobuf::RpcController* controller, const google::protobuf::Message* request,
                          google::protobuf::Message* response, google::protobuf::Closure* done)
{
    // 收集错误信息
    char errText[512] = {0};
    const google::protobuf::ServiceDescriptor* sd = method->service();
    string service_name = sd->name();
    string method_name = method->name();
    uint32_t args_size = 0;
    string args_str;
    if (request->SerializeToString(&args_str))
    {
        args_size = args_str.size();
    }
    else
    {
        controller->SetFailed("Serialize request error!");
        return;
    }
    mprpc::RpcHeader rpcHeader;
    uint32_t header_size = 0;
    string rpc_header_str;
    rpcHeader.set_service_name(service_name);
    rpcHeader.set_method_name(method_name);
    rpcHeader.set_args_size(args_size);
    if (rpcHeader.SerializeToString(&rpc_header_str))
    {
        header_size = rpc_header_str.size();
    }
    else
    {
        controller->SetFailed("Serialize rpc header error!");
        return;
    }
    // 组织待发送的数据
    string send_str;
    // send_str.insert(0, string((char*)header_size, 4));
    uint32_t net_header_size = htonl(header_size);  // 主机字节序 -> 网络字节序


    send_str.insert(0, string(reinterpret_cast<char*>(&net_header_size), 4));
    send_str += rpc_header_str;
    send_str += args_str;

    std::cout << "======================================\n";
    std::cout << "rpc_header_str: " << rpc_header_str << "\n";
    std::cout << "header_size: " << header_size << "\n";
    std::cout << "service_name: " << service_name << "\n";
    std::cout << "method_name: " << method_name << "\n";
    std::cout << "args_size: " << args_size << "\n";
    std::cout << "args_str: " << args_str << "\n";
    std::cout << "======================================\n";

    // 使用tcp编程，完成rpc方法的远程调用
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == clientfd)
    {
        sprintf(errText, "create socket error! errno: %d", errno);
        controller->SetFailed(errText);

        exit(EXIT_FAILURE);
    }
    // 读取配置文件rpcserver的信息
    // string ip = MprpcApplication::getInstance().getConfig().Load("rpcserverip");
    // uint16_t port = atoi(MprpcApplication::getInstance().getConfig().Load("rpcserverport").c_str());
    
    // rpc调用方法调用service_name 的method_name服务，需要查询zk上该服务所在的host信息
    ZkClient zkCli;
    zkCli.start();
    std::string method_path = "/" + service_name + "/" + method_name;
    std::string host_data = zkCli.getData(method_path.c_str());
    if (host_data == "")
    {
        controller->SetFailed(method_path + " is not exists!");
        return;
    }
    int idx = host_data.find(":");
    if (idx == -1)
    {
        controller->SetFailed(method_path + " address is invalid!");
        return;
    }
    std::string ip = host_data.substr(0, idx);
    uint16_t port = atoi(host_data.substr(idx + 1, host_data.size() - idx).c_str());

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

    // 连接rpc服务节点
    if (-1 == connect(clientfd, (struct sockaddr*)&server_addr, sizeof(server_addr)))
    {
        memset(errText, 0, sizeof(errText));
        sprintf(errText, "connect error! errno: %d", errno);
        controller->SetFailed(errText);
        close(clientfd);
        exit(EXIT_FAILURE);
    }

    if (-1 == send(clientfd, send_str.c_str(), send_str.size(), 0))
    {
        memset(errText, 0, sizeof(errText));
        sprintf(errText, "send error! errno: %d", errno);
        controller->SetFailed(errText);
        close(clientfd);
        return;
    }

    char buff[1024] = {0};
    int recv_size = 0;
    if (-1 == (recv_size = recv(clientfd, buff, 1024, 0)))
    {
        memset(errText, 0, sizeof(errText));
        sprintf(errText, "recv error! errno:  %d", errno);
        controller->SetFailed(errText);
        close(clientfd);
        return;
    }
    // string response_str(buff, 0, recv_size);
    // if (response->ParseFromString(response_str))
    if (response->ParseFromArray(buff, recv_size))
    {
        std::cout << "buff: " << buff << "\n";
        close(clientfd);
        return;
    }
    close(clientfd);
}