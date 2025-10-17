#include "mprpcconfig.h"
#include <iostream>
#include <stdio.h>
#include <unistd.h>

void MprpcConfig::Trim(std::string &str)
{
    auto l = str.find_first_not_of(' ');
    auto r = str.find_last_not_of(' ');

    if (l == std::string::npos) {
        str.clear();  // 全是空格
    } else {
        str = str.substr(l, r - l + 1);
    }
    
}

//负责解析加载配置文件
void MprpcConfig::LoadConfigFile(const char *config_file)
{
    FILE *pf = fopen(config_file, "r");
    if (pf == nullptr)
    {
        std::cout << config_file << " file is not exist!\n";
        exit(EXIT_FAILURE);
    }

    while(!feof(pf))
    {
        char buf[512] = {0};
        if (nullptr == fgets(buf, 512, pf))
        {
            std::cout << "fgets file error!\n";
            exit(EXIT_FAILURE);
        }
        std::string src_buf(buf);
        if(src_buf[0] == '#' ||  src_buf.empty())
        {
            continue;
        }
        Trim(src_buf);
        // 解析配置项
        int idx = src_buf.find('=');

        if(idx == -1)
        {
            // 配置不合法
            continue;
        }

        std::string key;
        std::string value;
        key = src_buf.substr(0, idx);
        Trim(key);

        int endidx = src_buf.find('\n');
        value = src_buf.substr(idx + 1, endidx - idx - 1);
        Trim(value);
        m_configMap[key] = value;
    }

    fclose(pf);
}
// 查询配置项信息
std::string MprpcConfig::Load(const std::string &key)
{
    static const std::string empty = "";
    auto it = m_configMap.find(key);
    if (it == m_configMap.end())
    {
        return "";
    }
    return it->second;
}