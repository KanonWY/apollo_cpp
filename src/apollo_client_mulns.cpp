#include "apollo_client_mulns.h"
#include "const_url_value.h"
#include <spdlog/spdlog.h>

namespace apollo_client
{

void apollo_mulns_client::init(const std::string &address,
                               const std::string &appid,
                               const std::string &clusterName,
                               const std::vector<std::string> &nsvec,
                               const apollo_client::apollo_mulns_client::Callback &cb,
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
    b_call_back = triggle_call_back;

    // first time update config map
    updateConfigMap();
    start_ = true;
    //start async thread to check
    submitNotificationsAsync();
}

void apollo_mulns_client::updateConfigMapByNamespace(const std::string &ns_name)
{
    {
        std::lock_guard<std::mutex> lock(ns_config_map_mt_);
        ns_config_map_[ns_name] = getConfigNoBufferInner(env_.address_, env_.appid_name_, ns_name, env_.cluster_name_);
    }
    {
        if (b_call_back) {
            call_back_();
        }
    }
}

void apollo_mulns_client::updateConfigMap()
{
    {
        std::lock_guard<std::mutex> lock(ns_config_map_mt_);
        for (const auto &ns : env_.ns_map_) {
            ns_config_map_.insert({ns, getConfigNoBufferInner(env_.address_, env_.appid_name_, ns, env_.cluster_name_)});
        }
    }
    if (b_call_back) {
        call_back_();
    }
}

void apollo_mulns_client::submitNotificationsAsync()
{
    if (start_) {
        loop_thread_ = std::thread(&apollo_mulns_client::submitNotificationFunc, this);
    }
}

web::http::status_code apollo_mulns_client::checkNotify()
{
    char stack_buf[200] = {0};
    sprintf(stack_buf, NOTIFICATIONS_URL, env_.address_.c_str());
    std::string base_url(stack_buf);
    try {
        web::http::client::http_client_config client_config;
        client_config.set_timeout(std::chrono::seconds(61));
        auto request_client = web::http::client::http_client(base_url, client_config);

        web::json::value notify_json_values;
        int json_array_index = 0;
        for (const auto &map_item : ns_notifyId_map_) {
            web::json::value tmp_json_value;
            tmp_json_value[U(NAMESPACE_STRING_NAME)] = web::json::value::string(map_item.first);
            tmp_json_value[U(NOTIFY_STRING_NAME)] = web::json::value::number(map_item.second);
            notify_json_values[U(TMP_JSON_KEY_STRING_NAME)][json_array_index] = tmp_json_value;
            json_array_index++;
        }
        auto ns_json_value = notify_json_values[U(TMP_JSON_KEY_STRING_NAME)];
        SPDLOG_INFO("checkNotify local info: {}", ns_json_value.serialize().c_str());

        web::http::uri_builder builder;
        builder.append_query(U(QUERY_PARAM_NOTIFY_STRING_NAME), ns_json_value.serialize().c_str());
        builder.append_query(U(QUERY_PARAM_APPID_STRING_NAME), env_.appid_name_.c_str());
        builder.append_query(U(QUERY_PARAM_CLUSTER_STRING_NAME), env_.cluster_name_.c_str());

        SPDLOG_INFO("check url = {} {}", base_url, builder.to_string().c_str());

        auto response = request_client.request(web::http::methods::GET, builder.to_string()).get();

        if (response.status_code() == web::http::status_codes::OK) {
            //update notifyId
            auto json_data = response.extract_json().get();
            auto json_array = json_data.as_array();
            // json_array = [{"messages":{"details":{"cp1+default+p1_pram_set":22}},"namespaceName":"p1_pram_set","notificationId":22},
            // {"messages":{"details":{"cp1+default+p2_pram_set":17}},"namespaceName":"p2_pram_set","notificationId":17}]
            //fill info to local set.
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
    }
    return 0;
}

// async thread check the notifyId changed
void apollo_mulns_client::submitNotificationFunc()
{
    while (start_) {
        auto res_status_code = checkNotify();
        if (res_status_code == web::http::status_codes::NotModified) {
            continue;
        } else if (res_status_code == web::http::status_codes::OK) {
            // update all
            updateConfigMap();
        } else {
            SPDLOG_ERROR("checkNotify case by other error, errorCode = {}", res_status_code);
            break;
        }
    }
}

//call back setting
void apollo_mulns_client::setCallback(const Callback &cb)
{
    call_back_ = cb;
}

void apollo_mulns_client::setCallback(Callback &&cb)
{
    call_back_ = std::move(cb);
}

void apollo_mulns_client::turnonCallback()
{
    b_call_back = true;
}

void apollo_mulns_client::turnoffCallback()
{
    b_call_back = false;
}

} // namespace apollo_client
