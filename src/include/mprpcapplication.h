#pragma once

#include "mprpcconfig.h"
#include "mprpcchannel.h"
#include "mprpccontroller.h"
// mprpc框架

class MprpcApplication
{
public:
    static void Init(int argc, char ** argv);
    static MprpcApplication& getInstance();
    static MprpcConfig& getConfig();
private:
    static MprpcConfig m_config;
    // 私有构造函数
    MprpcApplication() = default;
    MprpcApplication(const MprpcApplication&) = delete;
    MprpcApplication(MprpcApplication&&) = delete;
    MprpcApplication operator=(const MprpcApplication&) = delete;
    MprpcApplication& operator=(MprpcApplication&&) = delete;
};