#ifndef __APOLLO_CLIENT_MULNS_H__
#define __APOLLO_CLIENT_MULNS_H__

#include "apollo_base.h"
#include <set>
#include <atomic>
#include <mutex>
#include <thread>
#include <functional>
#include <vector>

#include <cpprest/http_client.h>

namespace apollo_client
{
class apollo_mulns_client : public apollo_base
{
public:
    using Callback = std::function<void()>;
    struct env_base
    {
        std::string address_{"http://localhost:8080"};
        std::string appid_name_;
        std::string cluster_name_{"default"};
        std::set<std::string> ns_map_; //a appid maybe have multi ns
    };

public:
    void init(
        const std::string &address,
        const std::string &appid,
        const std::string &clusterName,
        const std::vector<std::string> &nsvec,
        const Callback &c = []() {},
        bool triggle_call_back = false);

private:
    void updateConfigMapByNamespace(const std::string &ns_name);

    void updateConfigMap();

    void submitNotificationsAsync();

    void submitNotificationFunc();

    web::http::status_code checkNotify();



public:
    //call back setting
    void setCallback(const Callback& cb);

    void setCallback(Callback&& cb);

    void turnonCallback();
    
    void turnoffCallback();

    std::map<std::string, std::string> getConfigureByNs(const std::string& ns);


private:
    env_base env_;
    std::map<std::string, std::map<std::string, std::string>> ns_config_map_; //nsname {key1:value1, key2:value2, ...}
    std::mutex ns_config_map_mt_;
    std::map<std::string, int32_t> ns_notifyId_map_; //nsname : notifyID
    std::atomic<bool> start_{false};
    std::atomic<bool> b_call_back{false};
    Callback call_back_;
    std::thread loop_thread_;
};
} // namespace apollo_client

#endif