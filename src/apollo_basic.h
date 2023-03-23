#ifndef __APOLLO_BASIC_H__
#define __APOLLO_BASIC_H__

#include <cstdint>
#include <map>
#include <string>
#include <mutex>
#include <atomic>
#include <iostream>
#include <thread>

/**
 *  @brief Properties is Data store
 *
 * */
struct Properties
{
    std::map<std::string, std::string> kvmap_;
};

void DumpProperties(const Properties& pro)
{
    for (const auto& item : pro.kvmap_)
    {
        std::cout << "key = " << item.first << ". value = " << item.second
                  << std::endl;
    }
}

/**
 *  @brief  apolloEnv is basic env
 *
 * **/

struct apolloEnv
{
    std::string address_ = "http://localhost:8080";
    std::string appid_name_ = "codereview";
    std::string cluster_name_ = "default";
    std::string namespace_name_ = "codereviwNS";
};

// 获取特定环境，特定key的信息
std::string getNoCachePropertyString(const apolloEnv& env,
                                     const std::string& key);

// 获取不带缓存的配置信息，针对与某一个环境
Properties getNoCacheProperty(const apolloEnv& env);

// apollo客户端类
class ApolloClient
{
public:
    // Sig
    static ApolloClient& Instance()
    {
        static ApolloClient _instance;
        return _instance;
    }

    void startLongPoll();

    void stopLongPoll();

    void longPollFunc();

    void checkNotification();




    // no copy no move
    ApolloClient(const ApolloClient&) = delete;
    ApolloClient(ApolloClient&&) = delete;
    ApolloClient& operator=(const ApolloClient&) = delete;
    ApolloClient& operator=(ApolloClient&&) = delete;

    Properties getPropertiesNoBuffer();

    std::string getValueNoBuffer(const std::string& key);

    using NameSpaceNotifyUIDMap = std::map<std::string, int>;

private:
    ApolloClient() = default;

    NameSpaceNotifyUIDMap name_nid_map_;
    std::atomic<bool> b_long_poll_{false};

    apolloEnv nev_param_;
    Properties properties_;
    std::mutex mutex_;
    std::thread long_poll_thread_;
};

// some url pattern
const char* NO_BUFFER_URL = "%s/configs/%s/%s/%s";
const char* CONFIGURATIONS_NAME = "configurations";
const char* NOTIFICATIONS_URL = "%s/notifications/v2?appId=%s&cluster=%s&notifications=%d";

#endif
