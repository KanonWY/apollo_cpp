#ifndef __APOLLO_CLIENT_SINGLENS_H__
#define __APOLLO_CLIENT_SINGLENS_H__

#include "apollo_base.h"
#include <functional>
#include <mutex>
#include <cpprest/http_client.h>
#include <thread>

namespace apollo_client
{

// only one namespace for a client
class apollo_sgns_client : public apollo_base
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

    apollo_sgns_client() = default;

    void init(
        const std::string &address,
        const std::string &appid,
        const std::string &clusterName,
        const std::string &namespaceName,
        const Callback &c = []() {},
        bool triggle_call_back = false);

//    void init(
//        const std::string &address,
//        const std::string &appid,
//        const std::string &clusterName,
//        const std::string &namespaceName,
//        Callback &&cb = []() {},
//        bool triggle_call_back = false);

    void setCallback(const Callback &cb);

    void setCallback(Callback &&cb);

    void turnonCallback();

    void turnoffCallback();

    std::string getConfigNoBufferByKey(const std::string &key);

    std::string getConfigLocalBufferByKey(const std::string &key);

    ~apollo_sgns_client() override
    {
        start_ = false;
        if (loop_thread_.joinable()) {
            loop_thread_.join();
        }
        std::cout << "exit======================>" << std::endl;
    }

    void dumpConfig();

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

}; // namespace apollo_client

#endif