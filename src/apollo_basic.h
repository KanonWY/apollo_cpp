#ifndef __APOLLO_BASIC_H__
#define __APOLLO_BASIC_H__

#include <cstdint>
#include <map>

/**
 *  @brief Properties is Data store
 *
 * */
struct Properties
{
    std::map<std::string, std::string> kvmap_;
};

/**
 *  @brief  apolloEnv is basic env
 *
 * **/

struct apolloEnv
{
    std::string address_;        // 服务器IP
    std::string appid_name_;     // 应用名称
    std::string cluster_name_;   // 集群名称, 默认为default
    std::string namespace_name_; // 命名空间名称, 默认为application
};

struct Notification
{
    std::string namespace_name_;
    int notification_id_;
};
std::string getNoCachePropertyString(const apolloEnv& env,
                                     const std::string& key);

Properties getNoCacheProperty(const apolloEnv& env);
#endif
