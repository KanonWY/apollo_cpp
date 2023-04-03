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
#include <spdlog/spdlog.h>

/**
 * @brief function for debug.
 * @param property
 */
void DumpProperties(const std::map<std::string, std::string> &property)
{
    for (const auto &item : property) {
        std::cout << "key = " << item.first << ". value = " << item.second
                  << std::endl;
    }
}

namespace apollo_client
{

/**
 * @brief base apollo environment base, one appid -> one namespace.
 */
struct apolloEnv
{
    std::string address_ = "http://localhost:8080";
    std::string appid_name_ = "codereview";
    std::string cluster_name_ = "default";
    std::string namespace_name_ = "codereviwNS";
};

/**
 * @brief  for test.
 */
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

} // namespace apollo_client
#endif
