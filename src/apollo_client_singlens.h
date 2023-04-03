#ifndef __APOLLO_CLIENT_SINGLENS_H__
#define __APOLLO_CLIENT_SINGLENS_H__

#include "apollo_base.h"
#include <functional>
#include <mutex>
#include <cpprest/http_client.h>
#include <thread>

namespace apollo_client
{

/**
 * @brief apollo client: control one namespace.
 *        configType: properties.
 */
class apollo_sgns_client : public apollo_base
{
public:
    using Callback = std::function<void()>;
    /**
     * @brief base environment
     */
    struct env_base
    {
        std::string address_ = "http://localhost:8080";
        std::string appid_name_;
        std::string cluster_name_ = "default";
        std::string namespace_name_ = "application";
    };

    apollo_sgns_client() = default;

    /**
     * @brief init all env and start thread loop.
     * @param address
     * @param appid
     * @param clusterName
     * @param namespaceName
     * @param c
     * @param triggle_call_back
     */
    void init(
        const std::string &address,
        const std::string &appid,
        const std::string &clusterName,
        const std::string &namespaceName,
        const Callback &c = []() {},
        bool triggle_call_back = false);

    /**
     * @brief set call back
     * @param cb
     */
    void setCallback(const Callback &cb);

    void setCallback(Callback &&cb);

    /**
     * @brief turn on callback
     */
    void turnonCallback();

    /**
     * @brief turn off callback
     */
    void turnoffCallback();

    /**
     * @brief get config from db by key.(use a new http request to get config)
     * @param key
     * @return
     */
    std::string getConfigNoBufferByKey(const std::string &key);

    /**
     * @brief get config from local map buffer.(maybe older data)
     * @param key
     * @return
     */
    std::string getConfigLocalBufferByKey(const std::string &key);

    /**
     * @brief get config from local map.(maybe older data)
     * @return
     */
    std::map<std::string, std::string> getConfig();

    ~apollo_sgns_client() override
    {
        start_ = false;
        if (loop_thread_.joinable()) {
            loop_thread_.join();
        }
    }

    /**
     * @brief for debug
     */
    void dumpConfig();

private:
    /**
     * @brief update the config map.(use multi http request to get data from db)
     * @return
     */
    bool updateConfigMap();

    /**
     * @brief update config by key.
     * @param key
     */
    void updateConfigMapByKey(const std::string &key);

    /**
     * @brief check the notifyId.
     * @return
     */
    web::http::status_code checkNotify();

    /**
     * @brief async update notifyId.
     */
    void submitNotificationsAsync();

    /**
     * @brief thread loop function.
     */
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