#include "mprpcapplication.h"
#include <iostream>
#include <unistd.h>

using std::cout;
using std::endl;

MprpcConfig MprpcApplication::m_config;

void ShowArgsHelp()
{
    cout << "format: command -i <configfile>" << endl;
}

void MprpcApplication::Init(int argc, char ** argv)
{
    if(argc < 2)
    {
        ShowArgsHelp();
        exit(EXIT_FAILURE);
    }

    int c = 0;
    std::string config_file;
    std::string param;
    while ((c = getopt(argc, argv, "i:n:")) != -1)
    {
        switch (c)
        {
        case 'i':
            config_file = optarg;
            // 开始加载配置文件 rpcserver_ip = rpcserver_port = 
            m_config.LoadConfigFile(config_file.c_str()); 
            break;
        case '?':
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        case ':':
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        case 'n':
            param = optarg;
            std::cout << param << " value: " << m_config.Load(param) << std::endl;

        default:
            break;
        }
    }
}


MprpcApplication& MprpcApplication::getInstance() {
    static MprpcApplication app;
    return app;
}

MprpcConfig& MprpcApplication::getConfig()
{
    return m_config;
}