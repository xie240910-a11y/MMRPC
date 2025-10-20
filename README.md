# MMRPC学习

# 集群：每台服务器运行相同的程序，一个服务器宕机并不会影响其他程序继续执行
# 分布式：将功能进行分割，保证一个功能出现问题不会影响整个程序运行，同时也保证更新一个模块，不需要重新编译整个模块

# protobuf数据结构特点：
    1. 二进制存储，xml跟json都是文本存储
    2. protobuf不需要存储额外得信息
项目分布：
    bin：可执行文件
    build：项目编译文件
    lib：项目库文件l'lll
    src：源文件
    test：测试代码
    example：框架代码使用规范
    CMakeLists：顶层cmake文件
    README.md：项目自述文件
    autobuild.sh：一键部署脚本 

# proto常用方法：
    SerializeToString(*string)  序列号字符串
    ParseFromString(string) 反序列化Log对象
    setname() 设置属性
    name()    获取属性

    定义数据：message 类型名称（类似结构体对象形式）{int32, uint32, string, bytes, bool}
    设置嵌套属性：mutable_result(); //返回值是一个嵌套属性指针
    定义枚举： enum 枚举名称 {}
    定义列表：repeated
    添加列表属性：add_friend_list // 返回值是一个嵌套属性指针
    获取列表大小：friend_list_size()
    获取列表内元素：friend_list(int index)// 返回是一个列表内元素
    映射：map<string, int32> test;

    ## protobuf中定义rpc方法类型 -- service 
    // 默认不会生成service方法 需要添加：option cc_generic_services = true;
    service UserServiceRpc
    {

    }
    使用protoc会生成UserServiceRpc（rpc服务提供者）跟UserServiceRpc_Stub(rpc服务消费者)，Stub是一个装类，给调用方提供
    UserServiceRpc_Stub(RpcChannel *channel)构造方法，底层都是RpcChannel调用CallMethod()方法
    RpcChannel是一个抽象类，CallMethod是一个纯虚函数

# zk使用说明
    环境需要存在java的jdk
    解压压缩包 更新conf/zooexample.cfg 为zoo.cfg 更改文件的保存地址dataDir bin目录启动./zkServer.sh start, 进入客户端./zkCli.sh
    
    客户端常用命令：cd，ls，get，create，delete，set
