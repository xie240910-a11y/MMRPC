#pragma once

class MprpcApplication
{
public:
    static void Init(int argc, char const **argv);
    static MprpcApplication &GetInstance();
private:
    MprpcApplication(){};
    MprpcApplication(const MprpcApplication&) = delete;
    MprpcApplication(MprpcApplication&&) = delete;

};