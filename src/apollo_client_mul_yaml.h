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

/**
 * @brief  a class for one appid and multi namespace.
 */

class apollo_mul_yaml_client : public apollo_base
{
public:
    using Callback = std::function<void()>;

    /**
     * @brief environment base struct
     */
    struct env_base
    {
        std::string address_{"http://localhost:8080"};
        std::string appid_name_;
        std::string cluster_name_{"default"};
        std::set<std::string> ns_map_; //a appid maybe have multi ns
    };

public:
    /**
     * @brief init apollo_mul_yaml_client.
     * @param address
     * @param appid
     * @param clusterName
     * @param nsvec a vector to store namespace string
     * @param c
     * @param triggle_call_back
     */
    void init(
        const std::string &address,
        const std::string &appid,
        const std::string &clusterName,
        const std::vector<std::string> &nsvec,
        const Callback &c = []() {},
        bool triggle_call_back = false);

    /**
     * @brief get a yaml node config by namespace name.
     * @param ns_name
     * @return
     */
    YAML::Node getNsNameConfigNode(const std::string &ns_name);

    /**
     * @brief get all namespace string vector
     * @return
     */
    std::vector<std::string> getAllNamespaces();

    /**
     * @brief turn on the callback
     */
    void turnonCallback();

    /**
     * @brief turn off the callback
     */
    void turnoffCallback();

    /**
     * @brief set a callback.
     * @param cb
     */
    void setCallback(Callback &&cb);

    void setCallback(const Callback &cb);

    ~apollo_mul_yaml_client() override;

private:
    /**
     * @brief inner update the config map.
     */
    void updateYamlConfigMap();

    /**
     * @brief async update notifyId.
     */
    void submitNotificationsAsync();

    /**
     * @brief thread loop function.
     */
    void submitNotificationFunc();

    /**
     * @brief check the notifyId.
     * @return
     */
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
    inline static int reconnect_times_ = 5;
};

}; // namespace apollo_client

#endif