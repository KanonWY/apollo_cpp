#include "basemessage.h"
#include <cstring>
#include <vector>
#include <iostream>

std::vector<char> serialize_MsgBase(TcpHeader &data)
{
    std::vector<char> buf;
    buf.resize(sizeof(data.length) + data.content.ByteSize());
    std::cout << "buf size =  " << buf.capacity() << std::endl;
    std::string tp = data.content.SerializeAsString();
    data.length = tp.size();
    memcpy(buf.data(), &data.length, sizeof(data.length));
    memcpy(buf.data() + sizeof(data.length), tp.data(), tp.size());
    buf.push_back('\n');
    return buf;
}

void dumpContentInfo(const EASY_TCP::MsgContent &content)
{
    std::cout << "version = " << content.version() << std::endl;
    std::cout << "cmd = " << content.cmd() << std::endl;
    EASY_TCP::RequestContent real_content;
    auto it = content.content();
    std::cout << "it'size = " << it.size() << std::endl;
    real_content.ParseFromString(it);
    auto ts = real_content.timestamp();
    auto seq = real_content.sequence();
    auto host_info = real_content.host_info();
    auto token = real_content.token();
    auto appid = real_content.appid();
    std::cout << "timestamp = " << ts << std::endl;
    std::cout << "sequence = " << seq << std::endl;
    std::cout << "host_info = " << host_info.c_str() << std::endl;
    std::cout << "token = " << token.c_str() << std::endl;
    std::cout << "appid = " << appid.c_str() << std::endl;
}

int64_t getTimestampNow()
{
    auto now = std::chrono::system_clock::now();
    std::time_t re = std::chrono::system_clock::to_time_t(now);
    return re;
}