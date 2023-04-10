#ifndef APOLLO_CPP_CLIENT_APOLLO_BASE_CLIENT_H
#define APOLLO_CPP_CLIENT_APOLLO_BASE_CLIENT_H

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <thread>

#include <cpprest/http_client.h>
#include <spdlog/spdlog.h>
#include <tinyxml2.h>

#include "apollo_base.h"

namespace apollo_client
{

enum NAMESPACE_TYPE
{
    SINGLE_NAMESPACE,
    MULTI_NAMESPACE,
};

enum STORE_TYPE
{
    STRING_MAP,
};

template <NAMESPACE_TYPE T>
struct namespace_container
{
};

template <>
struct namespace_container<SINGLE_NAMESPACE>
{
    using NAMESPACE_TYPE = std::string;
};

template <>
struct namespace_container<MULTI_NAMESPACE>
{
    using NAMESPACE_TYPE = std::vector<std::string>;
};

template <NAMESPACE_TYPE T>
struct notify_container
{
};

template <>
struct notify_container<SINGLE_NAMESPACE>
{
    using NOTIFY_CONTAINER_TYPE = std::tuple<std::string, int32_t>;
};

template <>
struct notify_container<MULTI_NAMESPACE>
{
    using NOTIFY_CONTAINER_TYPE = std::map<std::string, int32_t>;
};

template <STORE_TYPE T>
struct store_container
{
};

template <>
struct store_container<STRING_MAP>
{
    using STORE_CONTAINER_TYPE = std::map<std::string, std::string>;
};

template <NAMESPACE_TYPE T>
struct AppConfig
{
    using NAMESPACE_TYPE = typename namespace_container<T>::NAMESPACE_TYPE;

    std::string address_{"http://localhost:8080"};
    std::string appid_{"SampleApp"};
    std::string cluster_{"default"};
    std::string env_{"DEV"};
    NAMESPACE_TYPE namespace_;
    bool is_backup_config_ = false;
    std::string secret_{};
    AppConfig() = default;

    //method chain
    AppConfig &SetAddress(const std::string &address)
    {
        address_ = address;
        return *this;
    }

    AppConfig &SetAppid(const std::string &appid)
    {
        appid_ = appid;
        return *this;
    }

    AppConfig &SetCluster(const std::string &cluster)
    {
        cluster_ = cluster;
        return *this;
    }

    AppConfig &SetNamespace(NAMESPACE_TYPE ns)
    {
        namespace_ = ns;
        return *this;
    }

    AppConfig &AppendNamespace(const std::string &ns)
    {
        if constexpr (T == MULTI_NAMESPACE) {
            namespace_.push_back(ns);
        } else {
            namespace_ = ns;
        }
        return *this;
    }

    AppConfig &ClearNamespace()
    {
        if constexpr (T == MULTI_NAMESPACE) {
            namespace_.clear();
        } else {
            namespace_ = "";
        }
        return *this;
    }

    AppConfig &SetBackUp(bool flag)
    {
        is_backup_config_ = flag;
        return *this;
    }

    AppConfig &SetSecret(const std::string &secret)
    {
        secret_ = secret;
        return *this;
    }

    /**
     * @brief for debug.
     */
    void dumpInfo()
    {
        std::cout << "address = " << address_ << std::endl;
        std::cout << "appid = " << appid_ << std::endl;
        std::cout << "cluster_ = " << cluster_ << std::endl;
        std::cout << "env = " << env_ << std::endl;
        if constexpr (T == MULTI_NAMESPACE) {
            for (const auto &item : namespace_) {
                std::cout << "ns = " << item << std::endl;
            }
        } else {
            std::cout << "ns = " << namespace_ << std::endl;
        }
        std::cout << "is_backup_config_ = " << is_backup_config_ << std::endl;
        std::cout << "secret_ = " << secret_ << std::endl;
    }
};

using SingleNsConfig = AppConfig<SINGLE_NAMESPACE>;
using MultiNsConfig = AppConfig<MULTI_NAMESPACE>;

template <NAMESPACE_TYPE T1, STORE_TYPE T2>
class apollo_client_base
{
public:
    //    In template: no type named 'STORE_CONTAINER_TYPE' in 'apollo_client::store_container<STRING_MAP>'
    using NOTIFY_CONTAINER_TYPE = typename notify_container<T1>::NOTIFY_CONTAINER_TYPE;
    using STORE_CONTAINER_TYPE = typename store_container<T2>::STORE_CONTAINER_TYPE;
    enum STATUS
    {
        STOPPED,
        STARTING,
        STARTED,
        NET_ERR
    };

    void init(const AppConfig<T1> &config)
    {
        config_ = config;
        //1. update
        status_ = STARTING;
        updateAllConfig();
        status_ = STARTED;
        thread_start_ = true;
        startBackThread();
    }

    void stop()
    {
        thread_start_ = false;
        if (thread_.joinable()) {
            thread_.join();
        }
    }

    apollo_client_base() = default;

    ~apollo_client_base()
    {
        thread_start_ = false;
        status_ = STOPPED;
        if (thread_.joinable()) {
            thread_.join();
        }
    }

    std::string getConfigByKey(const std::string &key)
    {
        return container_[key];
    }

private:
    void updateAllConfig()
    {
        if constexpr ((T2 == STRING_MAP) && (T1 == MULTI_NAMESPACE)) {
            //multi namespace
            for (const auto &item : config_.namespace_) {
                updateOneNs(item);
            }
        } else if constexpr ((T2 == STRING_MAP) && (T1 == SINGLE_NAMESPACE)) {
            updateOneNs(config_.namespace_);
        } else {
            SPDLOG_INFO("UNIMPLICATION");
        }
    }

    void updateOneNs(const std::string &ns)
    {
        if constexpr (T2 == STRING_MAP) {
            if (apollo_client::ends_with(ns, std::string(".yaml"))) {
                auto node_str = apollo_client::apollo_base::getConfig<apollo_client::YamlPolicy>(config_.address_,
                                                                                                 config_.appid_,
                                                                                                 ns,
                                                                                                 config_.cluster_);
                if (node_str.has_value()) {
                    std::lock_guard<std::mutex> locker(container_mutex_);
                    auto node = YAML::Load(node_str.value());
                    //也许可以用递归优化
                    parseYamlNode(node, container_);
                } else {
                    SPDLOG_ERROR("{} have not config!", ns);
                }
            } else if (apollo_client::ends_with(ns, std::string(".xml"))) {
                auto xml_str = apollo_client::apollo_base::getConfig<apollo_client::XmlPolicy>(config_.appid_,
                                                                                               config_.appid_,
                                                                                               ns,
                                                                                               config_.cluster_);
                if (xml_str.has_value()) {
                    std::lock_guard<std::mutex> locker(container_mutex_);
                    tinyxml2::XMLDocument doc;
                    doc.Parse(xml_str->c_str());
                    //也许可以用递归优化
                    parseXmlNode(doc.RootElement(), container_);
                } else {
                    SPDLOG_ERROR("{} have not config!", ns);
                }
            } else {
                container_ = apollo_client::apollo_base::getConfigNoBufferInner(config_.appid_,
                                                                                config_.appid_,
                                                                                ns,
                                                                                config_.cluster_);
            }
        } else {
            SPDLOG_INFO("UNIMPLICATION");
        }
    }

    void startBackThread()
    {
        if (status_ == STARTED) {
            thread_start_ = true;
            thread_ = std::thread(&apollo_client_base::backThread, this);
        } else {
            SPDLOG_ERROR("can't start back thread!");
        }
    }

    void backThread()
    {
        while (thread_start_) {
            auto res_status_code = checkNotify();
            if (res_status_code == web::http::status_codes::NotModified) {
                reconnect_times_ = 5;
            } else if (res_status_code == web::http::status_codes::OK) {
                //some ns have changed, so need to update all ns config.
                reconnect_times_ = 5;
                updateAllConfig();
            } else {
                SPDLOG_ERROR("checkNotify case by other error, errorCode = {}", res_status_code);
                if (reconnect_times_ <= 0) {
                    break;
                } else {
                    SPDLOG_ERROR("start to reconnect! times {},....", reconnect_times_);
                    reconnect_times_--;
                }
            }
        }
    }

    web::http::status_code checkNotify()
    {
        auto base_url = get_notify_url(config_.address_);
        try {
            web::http::client::http_client_config client_config;
            client_config.set_timeout(std::chrono::seconds(62));
            auto request_client = web::http::client::http_client(base_url, client_config);

            web::json::value notify_json_values;

            // one namespace or multi namespace
            if constexpr (T1 == MULTI_NAMESPACE) {
                int json_array_index = 0;
                for (const auto &map_item : notify_container_) {
                    web::json::value tmp_json_value;
                    tmp_json_value[U("namespaceName")] = web::json::value::string(map_item.first);
                    tmp_json_value[U("notificationId")] = web::json::value::number(map_item.second);
                    notify_json_values[U("param")][json_array_index] = tmp_json_value;
                    json_array_index++;
                }
            } else { // one namespace, notify_container is tuple
                web::json::value tmp_json_value;
                tmp_json_value[U("namespaceName")] = web::json::value::string(std::get<0>(notify_container_));
                tmp_json_value[U("notificationId")] = web::json::value::number(std::get<1>(notify_container_));
                notify_json_values[U("param")][0] = tmp_json_value;
            }

            auto ns_json_value = notify_json_values[U("param")];
            web::http::uri_builder builder;
            builder.append_query(U("notifications"), ns_json_value.serialize().c_str());
            builder.append_query(U("appId"), config_.appid_.c_str());
            builder.append_query(U("cluster"), config_.cluster_.c_str());

            auto response = request_client.request(web::http::methods::GET, builder.to_string()).get();
            if (response.status_code() == web::http::status_codes::OK) {
                //some config have changed.
                auto json_data = response.extract_json().get();
                SPDLOG_INFO("json_data use yaml notify: {}", json_data.serialize().c_str());
                auto json_array = json_data.as_array();
                if constexpr (T1 == MULTI_NAMESPACE) {
                    for (auto &i : json_array) {
                        auto json_obj = i.as_object();
                        std::string ns_name;
                        int32_t notify_id = -1;
                        for (auto &item : json_obj) {
                            if (!strcmp(item.first.c_str(), "namespaceName")) {
                                ns_name = item.second.as_string();
                            } else if (!strcmp(item.first.c_str(), "notificationId")) {
                                notify_id = item.second.as_number().to_int32();
                            }
                        }
                        notify_container_[ns_name] = notify_id;
                    }
                } else {
                    auto json_obj = json_array[0].as_object();
                    std::string ns_name;
                    int32_t notify_id = -1;
                    for (auto &item : json_obj) {
                        if (!strcmp(item.first.c_str(), "namespaceName")) {
                            ns_name = item.second.as_string();
                        } else if (!strcmp(item.first.c_str(), "notificationId")) {
                            notify_id = item.second.as_number().to_int32();
                        }
                    }
                    std::get<0>(notify_container_) = ns_name;
                    std::get<1>(notify_container_) = notify_id;
                }
            } else if (response.status_code() == web::http::status_codes::NotModified) {
                SPDLOG_INFO("all config  notModified!");
            } else {
                SPDLOG_ERROR("checkNotify error: {} \n url = {} {}", response.status_code(), base_url, builder.to_string().c_str());
            }
            return response.status_code();
        } catch (std::exception &e) {
            SPDLOG_ERROR("Exception: {} ", e.what());
            if (!strcmp(e.what(), "Failed to read HTTP status line")) {
                //net block may cast read status line error!
                return web::http::status_codes::NotModified;
            } else {
                return web::http::status_codes::BadGateway;
            }
        }
    }

private:
    AppConfig<T1> config_;
    STATUS status_{STOPPED};
    NOTIFY_CONTAINER_TYPE notify_container_;
    std::mutex container_mutex_;
    STORE_CONTAINER_TYPE container_;
    std::thread thread_;
    bool thread_start_{false};
    short reconnect_times_{8};
};

using apollo_client_single_ns = apollo_client_base<apollo_client::SINGLE_NAMESPACE, apollo_client::STRING_MAP>;
using apollo_client_multi_ns = apollo_client_base<apollo_client::MULTI_NAMESPACE, apollo_client::STRING_MAP>;

} // namespace apollo_client

#endif //APOLLO_CPP_CLIENT_APOLLO_BASE_CLIENT_H
