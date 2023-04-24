#ifndef APOLLO_CPP_CLIENT_APOLLO_PARAM_BASE_H
#define APOLLO_CPP_CLIENT_APOLLO_PARAM_BASE_H

#include <string>
#include "../include/apollo_base_client.h"
#include "../include/apollo_openapi_client.h"
#include "../include/apollo_openapi_base.h"

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
    using type = apollo_client::apollo_ctrl_base;
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

template <typename BACK_SERVER, SERVER_TYPE S_TYPE>
class Base_Server
{
};

template <typename BACK_SERVER>
class Base_Server<BACK_SERVER, Q_CLIENT_MUL_NS>
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
    client_var<Q_CLIENT_MUL_NS>::type client_var_;
};

template <typename BACK_SERVER>
class Base_Server<BACK_SERVER, CLIENT_OPEN_API>
{
public:
    bool init(const param_server_config &config)
    {
        config.fillClientConfig(client_config_);
        server_ip_ = config.server_ip_;
        port_ = config.port;
        client_var_.init(client_config_.secret_, client_config_);
        return static_cast<BACK_SERVER *>(this)->initImp(port_, config.server_ip_);
    }

    virtual ~Base_Server() = default;

    std::optional<std::string> getStringConfig(const std::string &appid, const std::string &ns, const std::string &key)
    {
        auto value = client_var_.getConfig(appid, ns, key);
        if (value.has_value()) {
            std::stringstream ss;
            ss << value.value();
            return ss.str();
        } else {
            return {};
        }
    }

    std::optional<std::vector<std::string>> getAllKeys()
    {
        return client_var_.getAllKey();
    }

private:
    // apollo客户端服务类型
    apollo_client::MultiNsConfig client_config_;
    std::string server_ip_{};
    short port_{};

protected:
    client_var<CLIENT_OPEN_API>::type client_var_;
};

template <typename BACK_SERVER>
using Base_Server_1 = Base_Server<BACK_SERVER, Q_CLIENT_MUL_NS>;

template <typename BACK_SERVER>
using Base_Server_2 = Base_Server<BACK_SERVER, CLIENT_OPEN_API>;

} // namespace apollo_param

#endif //APOLLO_CPP_CLIENT_APOLLO_PARAM_BASE_H
