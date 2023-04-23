#ifndef APOLLO_CPP_CLIENT_APOLLO_PARAM_BASE_H
#define APOLLO_CPP_CLIENT_APOLLO_PARAM_BASE_H

#include <string>
#include "../include/apollo_base_client.h"
#include "../include/apollo_openapi_client.h"

namespace apollo_param
{

// 提供服务类型
enum SERVER_TYPE
{
    Q_CLIENT_MUL_NS = 0,
    Q_CLIENT_MUL_NS_NODE = 1,
    CLIENT_OPEN_API = 2
};

template <SERVER_TYPE type>
struct client_var
{
};

template <>
struct client_var<Q_CLIENT_MUL_NS>
{
    using type = apollo_client::apollo_client_multi_ns;
};

template <>
struct client_var<Q_CLIENT_MUL_NS_NODE>
{
    using type = apollo_client::apollo_client_multi_ns_node;
};

template <>
struct client_var<CLIENT_OPEN_API>
{
    using type = apollo_client::apollo_openapi_client_single;
};

#define SET_METHOD_CHAIN(var)                                         \
    inline param_server_config &set##var##value(const std::string &v) \
    {                                                                 \
        this->var = v;                                                \
        return *this;                                                 \
    }
struct param_server_config
{
    std::string server_ip_;
    short port;
    std::string apollo_config_ip_;
    std::string apollo_openapi_ip_;
    std::string appid_;
    std::string cluster_;
    std::string env_;
    std::string secret_;
    std::vector<std::string> namespaces_;

    // method chain

    SET_METHOD_CHAIN(server_ip_)

    SET_METHOD_CHAIN(apollo_config_ip_)

    SET_METHOD_CHAIN(apollo_openapi_ip_)

    SET_METHOD_CHAIN(appid_)

    SET_METHOD_CHAIN(cluster_)

    SET_METHOD_CHAIN(env_)

    SET_METHOD_CHAIN(secret_)

    inline param_server_config &setnamespace_value(const std::string &ns)
    {
        this->namespaces_.push_back(ns);
        return *this;
    }

    inline param_server_config &setport_value(short v)
    {
        this->port = v;
        return *this;
    }

    void fillClientConfig(apollo_client::MultiNsConfig &config) const;

    bool isValid() const;
};

template <typename BACK_SERVER>
class Base_Server
{
public:
    bool init(const param_server_config &config)
    {
        config.fillClientConfig(client_config_);
        server_ip_ = config.server_ip_;
        port_ = config.port;
        client_var_.init(client_config_);
        return static_cast<BACK_SERVER *>(this)->initImp(port_, config.server_ip_);
    }

    virtual ~Base_Server() = default;

    std::string getStringConfig(const std::string &key)
    {
        return client_var_.getConfigByKey(key);
    }

    std::vector<std::string> getAllKeys()
    {
        return client_var_.getConfigKeys();
    }

private:
    // apollo客户端服务类型
    apollo_client::MultiNsConfig client_config_;
    std::string server_ip_{};
    short port_{};

protected:
    apollo_client::apollo_client_multi_ns client_var_;
};
} // namespace apollo_param

#endif //APOLLO_CPP_CLIENT_APOLLO_PARAM_BASE_H
