#ifndef __APOLLO_CLIENT_MUL_YAML_H__
#define __APOLLO_CLIENT_MUL_YAML_H__

#include "apollo_base.h"
#include <mutex>
#include <thread>
#include <string>
#include <functional>

namespace YAML
{
class Node;
};

namespace apollo_client
{

class apollo_mul_yaml_client : public apollo_base
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

    YAML::Node getNsNameConfigNode(const std::string &ns_name);

    //call back

    void turnonCallback();

    void turnoffCallback();

    void setCallback(Callback &&cb);

    void setCallback(const Callback &cb);

private:
    void updateYamlConfigMap();

    void submitNotificationsAsync();

    void submitNotificationFunc();

    web::http::status_code checkNotify();

private:
    env_base env_;
    std::map<std::string, YAML::Node> ns_yaml_config_map_;
    std::map<std::string, int32_t> ns_notifyId_map_;
    std::mutex ns_yaml_config_mt_;
    std::thread loop_thread_;
    std::atomic<bool> start_{false};
    std::atomic<bool> b_call_back_{false};
    Callback call_back_;
};

}; // namespace apollo_client

#endif