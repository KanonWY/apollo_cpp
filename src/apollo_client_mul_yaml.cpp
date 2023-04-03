#include "apollo_client_mul_yaml.h"

namespace apollo_client
{

void apollo_mul_yaml_client::init(const std::string &address,
                                  const std::string &appid,
                                  const std::string &clusterName,
                                  const std::vector<std::string> &nsvec,
                                  const apollo_mul_yaml_client::Callback &cb,
                                  bool triggle_call_back)
{
    env_.appid_name_ = appid;
    env_.address_ = address;
    env_.cluster_name_ = clusterName;
    for (const auto &ns_item : nsvec) {
        env_.ns_map_.insert(ns_item);
        ns_notifyId_map_.insert({ns_item, -1});
    }
    call_back_ = cb;
    b_call_back_ = triggle_call_back;
    start_ = true;
    // first time to update the ConfigMap
    updateYamlConfigMap();
    // start async thread to check.
    submitNotificationsAsync();
}

YAML::Node apollo_mul_yaml_client::getNsNameConfigNode(const std::string &ns_name)
{
    //TODO lock?
    return ns_yaml_config_map_[ns_name];
}

void apollo_mul_yaml_client::updateYamlConfigMap()
{
    if (!start_) {
        SPDLOG_ERROR("updateYamlConfigMap exec before init!");
        return;
    }
    {
        std::lock_guard<std::mutex> lock(ns_yaml_config_mt_);
        // clear all content than get all new!
        ns_yaml_config_map_.clear();
        for (const auto &ns : env_.ns_map_) {
            ns_yaml_config_map_.insert({ns, getYamlConfig<RE_TYPE::YAML_OBJECT>(env_.address_, env_.appid_name_, ns, env_.cluster_name_)});
        }
    }
    if (b_call_back_) {
        call_back_();
    }
}

void apollo_mul_yaml_client::submitNotificationsAsync()
{
    if (start_) {
        loop_thread_ = std::thread(&apollo_mul_yaml_client::submitNotificationFunc, this);
    }
}

void apollo_mul_yaml_client::submitNotificationFunc()
{
    while (start_) {
        auto res_status_code = checkNotify();
        if (res_status_code == web::http::status_codes::NotModified) {
            continue;
        } else if (res_status_code == web::http::status_codes::OK) {
            //some ns have changed, so need to update all ns config.
            updateYamlConfigMap();
            // TODO: if one ns delete, it can notify?
        } else {
            SPDLOG_ERROR("checkNotify case by other error, errorCode = {}", res_status_code);
            break;
        }
    }
}

web::http::status_code apollo_mul_yaml_client::checkNotify()
{
    std::string base_url;
    {
        char *url = new char[266];
        sprintf(url, "%s/notifications/v2", env_.address_.c_str());
        base_url = url;
        delete[] url;
    }
    try {
        web::http::client::http_client_config client_config;
        client_config.set_timeout(std::chrono::seconds(61));
        auto request_client = web::http::client::http_client(base_url, client_config);

        web::json::value notify_json_values;
        int json_array_index = 0;
        for (const auto &map_item : ns_notifyId_map_) {
            web::json::value tmp_json_value;
            tmp_json_value[U("namespaceName")] = web::json::value::string(map_item.first);
            tmp_json_value[U("notificationId")] = web::json::value::number(map_item.second);
            notify_json_values[U("param")][json_array_index] = tmp_json_value;
            json_array_index++;
        }
        auto ns_json_value = notify_json_values[U("param")];
        SPDLOG_INFO("checkNotify local info: {}", ns_json_value.serialize().c_str());

        web::http::uri_builder builder;
        builder.append_query(U("notifications"), ns_json_value.serialize().c_str());
        builder.append_query(U("appId"), env_.appid_name_.c_str());
        builder.append_query(U("cluster"), env_.cluster_name_.c_str());

        SPDLOG_INFO("check url = {} {}", base_url, builder.to_string().c_str());

        //block 61s if no change
        auto response = request_client.request(web::http::methods::GET, builder.to_string()).get();
        if (response.status_code() == web::http::status_codes::OK) {
            //some config have changed.
            auto json_data = response.extract_json().get();
            SPDLOG_INFO("json_data use yaml notify: {}", json_data.serialize().c_str());
            auto json_array = json_data.as_array();
            for (auto &i : json_array) {
                auto json_abj = i.as_object();
                std::string ns_name;
                int32_t notify_id = -1;
                for (auto &item : json_abj) {
                    //nsname must before notifyId
                    if (!strcmp(item.first.c_str(), "namespaceName")) {
                        ns_name = item.second.as_string();
                    } else if (!strcmp(item.first.c_str(), "notificationId")) {
                        notify_id = item.second.as_number().to_int32();
                    }
                }
                ns_notifyId_map_[ns_name] = notify_id;
                SPDLOG_INFO("namespace = {} , notify_id = {}", ns_name.c_str(), notify_id);
            }
        }
        return response.status_code();
    } catch (std::exception &e) {
        SPDLOG_ERROR("Exception: {} ", e.what());
        return web::http::status_codes::BadRequest;
    }
}

void apollo_mul_yaml_client::turnonCallback()
{
    b_call_back_ = true;
}

void apollo_mul_yaml_client::turnoffCallback()
{
    b_call_back_ = false;
}

void apollo_mul_yaml_client::setCallback(Callback &&cb)
{
    call_back_ = std::move(cb);
}

void apollo_mul_yaml_client::setCallback(const Callback &cb)
{
    call_back_ = cb;
}
} // namespace apollo_client