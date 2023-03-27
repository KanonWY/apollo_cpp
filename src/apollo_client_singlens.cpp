#include "const_url_value.h"
#include "apollo_client_singlens.h"
#include <spdlog/spdlog.h>

namespace apollo_client
{
void apollo_sgns_client::init(
    const std::string &address,
    const std::string &appid,
    const std::string &clusterName,
    const std::string &namespaceName,
    const Callback &cb,
    bool triggle_call_back)
{
    env_.appid_name_ = appid;
    env_.address_ = address;
    env_.namespace_name_ = namespaceName;
    env_.cluster_name_ = clusterName;
    std::get<0>(nsNid_) = namespaceName;
    start_ = true;
    b_call_back = triggle_call_back;
    call_back_ = cb;
    updateConfigMap();
    dumpConfig();
    submitNotificationsAsync();
}

//void apollo_sgns_client::init(const std::string &address,
//                              const std::string &appid,
//                              const std::string &clusterName,
//                              const std::string &namespaceName,
//                              apollo_sgns_client::Callback &&cb,
//                              bool triggle_call_back)
//{
//    env_.appid_name_ = appid;
//    env_.address_ = address;
//    env_.namespace_name_ = namespaceName;
//    env_.cluster_name_ = clusterName;
//    std::get<0>(nsNid_) = namespaceName;
//    start_ = true;
//    b_call_back = triggle_call_back;
//    call_back_ = std::move(cb);
//    updateConfigMap();
//    dumpConfig();
//    submitNotificationsAsync();
//}

std::string apollo_sgns_client::getConfigNoBufferByKey(const std::string &key)
{
    if (!start_) {
        return {};
    }
    return getConfigNoBufferByKeyInner(env_.address_,
                                       env_.appid_name_,
                                       env_.namespace_name_,
                                       env_.cluster_name_,
                                       key);
}
bool apollo_sgns_client::updateConfigMap()
{
    bool res{false};
    {
        std::lock_guard<std::mutex> lock(config_map_mt_);
        res = getConfigNoBufferInner(
            env_.address_,
            env_.appid_name_,
            env_.namespace_name_,
            env_.cluster_name_,
            config_map_);
    }
    if (b_call_back) {
        call_back_();
    }
    return res;
}

void apollo_sgns_client::updateConfigMapByKey(const std::string &key)
{
    //TODO
}
web::http::status_code apollo_sgns_client::checkNotify()
{
    char stackBuf[200] = {0};
    sprintf(stackBuf, NOTIFICATIONS_URL, env_.address_.c_str());
    std::string base_url(stackBuf);
    try {
        //  config 61s time out because of the check success will keep 60s.
        web::http::client::http_client_config clientConfig;
        clientConfig.set_timeout(std::chrono::seconds(61));
        auto requestClient = web::http::client::http_client(base_url, clientConfig);

        // contact request param from name_nid_map_
        web::json::value NotifyJsonValue;
        int jsonArrayIndex = 0;
        // put map value into JsonArr
        web::json::value tmpJsonValue;
        tmpJsonValue[U(NAMESPACE_STRING_NAME)] = web::json::value::string(std::get<0>(nsNid_));
        tmpJsonValue[U(NOTIFY_STRING_NAME)] = web::json::value::number(std::get<1>(nsNid_));
        NotifyJsonValue[U(TMP_JSON_KEY_STRING_NAME)][jsonArrayIndex] = tmpJsonValue;
        auto nsNfJsonValue = NotifyJsonValue[U(TMP_JSON_KEY_STRING_NAME)];
        SPDLOG_INFO("checkNotify local info: {}", nsNfJsonValue.serialize().c_str());

        // use builder to fill the url
        web::http::uri_builder builder;
        builder.append_query(U(QUERY_PARAM_NOTIFY_STRING_NAME), nsNfJsonValue.serialize().c_str());
        builder.append_query(U(QUERY_PARAM_APPID_STRING_NAME), env_.appid_name_.c_str());
        builder.append_query(U(QUERY_PARAM_CLUSTER_STRING_NAME), env_.cluster_name_.c_str());

        SPDLOG_INFO("check url = {} {}",base_url, builder.to_string().c_str());

        // send request(it's a block way) and check return status
        auto response = requestClient.request(web::http::methods::GET, builder.to_string())
                            .get();
        if (response.status_code() == web::http::status_codes::OK) {
            //update notifyId
            auto jsonData = response.extract_json().get();
            auto jsonArrRes = jsonData.as_array()[0].as_object();
            for (auto item = jsonArrRes.begin(); item != jsonArrRes.end(); ++item) {
                if (item->second.type() == web::json::value::value_type::Number) {
                    std::get<1>(nsNid_) = item->second.as_number().to_int32();
                    SPDLOG_INFO("json from configServer: {} : {}", item->first.c_str(), item->second.as_number().to_int32());
                    break;
                }
            }
        }
        return response.status_code();
    } catch (std::exception &e) {
        SPDLOG_ERROR("Exception: {} ", e.what());
    }
    return web::http::status_codes::InternalError;
}

void apollo_sgns_client::submitNotificationsAsync()
{
    if (start_) {
        loop_thread_ = std::thread(&apollo_sgns_client::submitNotificationFunc, this);
    }
}

void apollo_sgns_client::submitNotificationFunc()
{
    while (start_) {
        auto resStatusCode = checkNotify();
        if (resStatusCode == web::http::status_codes::NotModified) {
            continue;
        } else if (resStatusCode == web::http::status_codes::OK) {
            // checkNotify already update notifyId, then updateConfigMap
            updateConfigMap();
        } else {
            SPDLOG_ERROR("checkNotify case by other error, errorCode = {}", resStatusCode);
            break;
        }
    }
}

std::string apollo_sgns_client::getConfigLocalBufferByKey(const std::string &key)
{
    std::lock_guard<std::mutex> lock(config_map_mt_);
    return config_map_[key];
}

void apollo_sgns_client::setCallback(const apollo_sgns_client::Callback &cb)
{
    call_back_ = cb;
}

void apollo_sgns_client::setCallback(apollo_sgns_client::Callback &&cb)
{
    call_back_ = std::move(cb);
}
void apollo_sgns_client::turnonCallback()
{
    b_call_back = true;
}
void apollo_sgns_client::turnoffCallback()
{
    b_call_back = false;
}
void apollo_sgns_client::dumpConfig()
{
    std::lock_guard<std::mutex> lock(config_map_mt_);
    for (const auto &item : config_map_) {
        SPDLOG_INFO("key = {}, value = {}", item.first.c_str(), item.second.c_str());
    }
}

} // namespace apollo_client