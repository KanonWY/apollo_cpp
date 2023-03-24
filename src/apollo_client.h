#ifndef __APOLLO_CLIENT_H__
#define __APOLLO_CLIENT_H__

#include <cstdint>
#include <map>
#include <string>
#include <mutex>
#include <atomic>
#include <iostream>
#include <thread>
#include <cpprest/http_client.h>
#include <tuple>
#include "apollo_base.h"

void DumpProperties(const std::map<std::string, std::string> &property)
{
    for (const auto &item : property) {
        std::cout << "key = " << item.first << ". value = " << item.second
                  << std::endl;
    }
}

struct apolloEnv
{
    std::string address_ = "http://localhost:8080";
    std::string appid_name_ = "codereview";
    std::string cluster_name_ = "default";
    std::string namespace_name_ = "codereviwNS";
};

// apollo客户端类
class ApolloClient : public apollo_base
{
public:
    // Sig
    static ApolloClient &Instance()
    {
        static ApolloClient _instance;
        return _instance;
    }

    using NameSpaceNotifyUIDMap = std::map<std::string, int>; //namespace -> notifyId
    using NotifyRe = std::tuple<web::http::status_code, NameSpaceNotifyUIDMap>;

    void startLongPoll();

    void stopLongPoll();

    void longPollFunc();

    void submitNotificationsAsync();

    NotifyRe checkNotificationAndRe();

    // no copy no move
    ApolloClient(const ApolloClient &) = delete;
    ApolloClient(ApolloClient &&) = delete;
    ApolloClient &operator=(const ApolloClient &) = delete;
    ApolloClient &operator=(ApolloClient &&) = delete;

    std::map<std::string, std::string> getPropertiesNoBuffer();

    std::string getValueNoBuffer(const std::string &key);

    void updatePropertiesNoBuffer();

    void testCheckNotify(const NameSpaceNotifyUIDMap &m);

private:
    ApolloClient() = default;

    NameSpaceNotifyUIDMap name_nid_map_;
    std::atomic<bool> b_long_poll_{false};

    apolloEnv env_param_;
    std::map<std::string, std::string> properties_;
    std::mutex mutex_;
    std::thread long_poll_thread_;
};

// only one namespace for a client
class ApolloClientSingleNs : public apollo_base
{
public:
    using Callback = std::function<void()>;
    struct env_base
    {
        std::string address_ = "http://localhost:8080";
        std::string appid_name_;
        std::string cluster_name_ = "default";
        std::string namespace_name_ = "application";
    };

    ApolloClientSingleNs() = default;

    void init(const std::string &address,
              const std::string &appid,
              const std::string &clusterName,
              const std::string &namespaceName,
              const Callback &c,
              bool triggle_call_back);

    void setCallback(const Callback &cb);

    void setCallback(Callback &&cb);

    void turnOnCallback();

    void turnOffCallback();

    std::string getConfigNoBufferByKey(const std::string &key);

    std::string getConfigLocalBufferByKey(const std::string &key);

    ~ApolloClientSingleNs() override
    {
        start_ = false;
        if (loop_thread_.joinable()) {
            loop_thread_.join();
        }
    }

private:
    bool updateConfigMap();

    void updateConfigMapByKey(const std::string &key);
    //    void updateConfigMapByKeyVector(); TODO

    web::http::status_code checkNotify();

    void submitNotificationsAsync();

    void submitNotificationFunc();

private:
    env_base env_;
    std::atomic<bool> start_{false};
    std::atomic<bool> b_call_back{false};
    std::map<std::string, std::string> config_map_;
    std::mutex config_map_mt_;
    std::tuple<std::string, int32_t> nsNid_{"", -1};
    std::thread loop_thread_;
    Callback call_back_;
};

class ApolloClientMultiNs : public apollo_base
{
public:
    using namespaceConfigMap = std::map<std::string, std::map<std::string, std::string>>;
    using namespaceNotifyMap = std::map<std::string, int32_t>;
    using Callback = std::function<void()>;
    struct env_base
    {
        std::string address_ = "http://localhost:8080";
        std::string appid_name_;
        std::string cluster_name_ = "default";
        std::vector<std::string> ns_name_vec_; //maybe exist multi namespace and multi config
    };

private:
    env_base env_;
    std::atomic<bool> start_{false};
    std::atomic<bool> b_call_back_{false};
    namespaceConfigMap ns_config_map_;
    std::mutex ns_config_map_mt_;
    namespaceNotifyMap ns_notify_map_;
    std::mutex ns_notify_map_mt_;
    std::thread loop_thread_;
    Callback call_back;
};

#endif
