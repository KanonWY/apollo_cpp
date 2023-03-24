#include <iostream>
#include <vector>
#include "apollo_client.h"
#include "apollo_base.h"
#include "const_url_value.h"

std::map<std::string, std::string> ApolloClient::getPropertiesNoBuffer()
{
    return {};
}

std::string ApolloClient::getValueNoBuffer(const std::string &key)
{
    return {};
}

void ApolloClient::startLongPoll() {}

void ApolloClient::stopLongPoll()
{
    b_long_poll_ = false;
    if (long_poll_thread_.joinable()) {
        long_poll_thread_.join();
    }
}
void ApolloClient::longPollFunc()
{
    // update local

    // loop
    while (b_long_poll_) {
    }
}

ApolloClient::NotifyRe ApolloClient::checkNotificationAndRe()
{
    ApolloClient::NotifyRe res;
    std::get<0>(res) = web::http::status_codes::InternalError;
    char stackBuf[200] = {0};
    sprintf(stackBuf, NOTIFICATIONS_URL, env_param_.address_.c_str());
    std::string Baseurl(stackBuf);
    try {
        //  config 61s time out because of the check success will keep 60s.
        web::http::client::http_client_config clientConfig;
        clientConfig.set_timeout(std::chrono::seconds(61));
        auto requestClient = web::http::client::http_client(Baseurl, clientConfig);

        // contact request param from name_nid_map_
        web::json::value NotifyJsonValue;
        int jsonArrayIndex = 0;
        // put map value into JsonArr
        for (const auto &mapItem : name_nid_map_) {
            web::json::value tmpJsonValue;
            tmpJsonValue[U(NAMESPACE_STRING_NAME)] = web::json::value::string(mapItem.first);
            tmpJsonValue[U(NOTIFY_STRING_NAME)] = web::json::value::number(mapItem.second);
            NotifyJsonValue[U(TMP_JSON_KEY_STRING_NAME)][jsonArrayIndex] = tmpJsonValue;
        }
        auto nsNfJsonValue = NotifyJsonValue[U(TMP_JSON_KEY_STRING_NAME)];
        std::cout << "jsonValue = " << nsNfJsonValue.serialize().c_str() << std::endl;

        // use builder to fill the url
        web::http::uri_builder builder;
        builder.append_query(U(QUERY_PARAM_NOTIFY_STRING_NAME), nsNfJsonValue.serialize().c_str());
        builder.append_query(U(QUERY_PARAM_APPID_STRING_NAME), env_param_.appid_name_.c_str());
        builder.append_query(U(QUERY_PARAM_CLUSTER_STRING_NAME), env_param_.namespace_name_.c_str());

        // send request(it's a block way) and check return status
        auto response = requestClient.request(web::http::methods::GET, builder.to_string())
                            .get();
        std::get<0>(res) = response.status_code();
        if (response.status_code() == web::http::status_codes::OK) {
            auto responseBodyJson = response.extract_json().get();
            for (int i = 0; i < responseBodyJson.size(); ++i) {
                std::get<1>(res).insert(
                    {responseBodyJson[i][U(NAMESPACE_STRING_NAME)].as_string(), responseBodyJson[i][U(NOTIFY_STRING_NAME)].as_number().to_int32()});
            }
        }
    } catch (std::exception &e) {
        std::cout << "Exception: " << e.what() << std::endl;
    }
    return res;
}

void ApolloClient::testCheckNotify(const NameSpaceNotifyUIDMap &nsNfMap)
{
    //1. get url
    std::string url = "http://localhost:8080/notifications/v2";
    try {

        //TODO add config
        web::http::client::http_client_config clientConfig;
        clientConfig.set_timeout(std::chrono::seconds(61));
        auto requestJson = web::http::client::http_client(url, clientConfig);

        //contact the url
        web::json::value NotifyJson;
        int jsonArrIndex = 0;
        for (const auto &mapItem : nsNfMap) {
            web::json::value tmpJsonValue;
            tmpJsonValue[U(NAMESPACE_STRING_NAME)] = web::json::value::string(mapItem.first);
            tmpJsonValue[U(NOTIFY_STRING_NAME)] = web::json::value::number(mapItem.second);
            NotifyJson[U(TMP_JSON_KEY_STRING_NAME)][jsonArrIndex] = tmpJsonValue;
            jsonArrIndex++;
        }
        auto nsNfJsonValue = NotifyJson[U(TMP_JSON_KEY_STRING_NAME)];
        std::cout << "jsonValue = " << nsNfJsonValue.serialize().c_str() << std::endl;

        //care: builder auto change url pram to url code.
        web::http::uri_builder builder;
        builder.append_query(U(QUERY_PARAM_NOTIFY_STRING_NAME), nsNfJsonValue.serialize().c_str());
        builder.append_query(U(QUERY_PARAM_APPID_STRING_NAME), "cp1");
        builder.append_query(U(QUERY_PARAM_CLUSTER_STRING_NAME), "default");

        auto response =
            requestJson.request(web::http::methods::GET, builder.to_string())
                .get();
        if (response.status_code() == web::http::status_codes::OK) {
            //parse the json data from http body, receive msg is json arr.
            ApolloClient::NameSpaceNotifyUIDMap returnMap;

            auto jsonData = response.extract_json().get();
            for (int i = 0; i < jsonData.size(); ++i) {
                auto item = jsonData[i];
                returnMap.insert({item[U(NAMESPACE_STRING_NAME)].as_string(), item[U(NOTIFY_STRING_NAME)].as_number().to_int32()});
            }

            for (const auto &item : returnMap) {
                std::cout << item.first << " " << item.second << std::endl;
            }

        } else if (response.status_code() == web::http::status_codes::NotModified) {
            std::cout << "nochange continue exec!" << std::endl;
        } else {
            std::cout << "Other status = " << response.status_code() << "! please check" << std::endl;
        }
    } catch (std::exception &e) {
        std::cout << "Exception: " << e.what() << std::endl;
    }
}
void ApolloClient::submitNotificationsAsync()
{
    long_poll_thread_ = std::thread(&ApolloClient::longPollFunc, this);
}
void ApolloClient::updatePropertiesNoBuffer()
{
}
void ApolloClientSingleNs::init(
    const std::string &address,
    const std::string &appid,
    const std::string &clusterName,
    const std::string &namespaceName,
    const Callback &cb = []() {},
    bool triggle_call_back = false)
{
    env_.appid_name_ = appid;
    env_.address_ = address;
    env_.namespace_name_ = namespaceName;
    env_.cluster_name_ = clusterName;
    std::get<0>(nsNid_) = namespaceName;
    start_ = true;
    b_call_back = triggle_call_back;
    updateConfigMap();
    submitNotificationsAsync();
}
std::string ApolloClientSingleNs::getConfigNoBufferByKey(const std::string &key)
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
bool ApolloClientSingleNs::updateConfigMap()
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

void ApolloClientSingleNs::updateConfigMapByKey(const std::string &key)
{
    //TODO
}
web::http::status_code ApolloClientSingleNs::checkNotify()
{
    char stackBuf[200] = {0};
    sprintf(stackBuf, NOTIFICATIONS_URL, env_.address_.c_str());
    std::string Baseurl(stackBuf);
    try {
        //  config 61s time out because of the check success will keep 60s.
        web::http::client::http_client_config clientConfig;
        clientConfig.set_timeout(std::chrono::seconds(61));
        auto requestClient = web::http::client::http_client(Baseurl, clientConfig);

        // contact request param from name_nid_map_
        web::json::value NotifyJsonValue;
        int jsonArrayIndex = 0;
        // put map value into JsonArr
        web::json::value tmpJsonValue;
        tmpJsonValue[U(NAMESPACE_STRING_NAME)] = web::json::value::string(std::get<0>(nsNid_));
        tmpJsonValue[U(NOTIFY_STRING_NAME)] = web::json::value::number(std::get<1>(nsNid_));
        NotifyJsonValue[U(TMP_JSON_KEY_STRING_NAME)][jsonArrayIndex] = tmpJsonValue;
        auto nsNfJsonValue = NotifyJsonValue[U(TMP_JSON_KEY_STRING_NAME)];
        std::cout << "jsonValue = " << nsNfJsonValue.serialize().c_str() << std::endl;

        // use builder to fill the url
        web::http::uri_builder builder;
        builder.append_query(U(QUERY_PARAM_NOTIFY_STRING_NAME), nsNfJsonValue.serialize().c_str());
        builder.append_query(U(QUERY_PARAM_APPID_STRING_NAME), env_.appid_name_.c_str());
        builder.append_query(U(QUERY_PARAM_CLUSTER_STRING_NAME), env_.namespace_name_.c_str());

        // send request(it's a block way) and check return status
        auto response = requestClient.request(web::http::methods::GET, builder.to_string())
                            .get();

        if (response.status_code() == web::http::status_codes::OK) {
            //update notifyId
            auto jsonData = response.extract_json().get();
            std::get<1>(nsNid_) = jsonData[0].as_number().to_int32();
        }
        return response.status_code();
    } catch (std::exception &e) {
        std::cout << "Exception: " << e.what() << std::endl;
    }
    return web::http::status_codes::InternalError;
}

void ApolloClientSingleNs::submitNotificationsAsync()
{
    if (start_) {
        loop_thread_ = std::thread(&ApolloClientSingleNs::submitNotificationFunc, this);
    }
}

void ApolloClientSingleNs::submitNotificationFunc()
{
    while (start_) {
        static auto resStatusCode = checkNotify();
        if (resStatusCode == web::http::status_codes::NotModified) {
            continue;
        } else if (resStatusCode == web::http::status_codes::OK) {
            // checkNotify already update notifyId, then updateConfigMap
            updateConfigMap();
        } else {
            std::cout << "checkNotify error" << std::endl;
            break;
        }
    }
}

std::string ApolloClientSingleNs::getConfigLocalBufferByKey(const std::string &key)
{
    std::lock_guard<std::mutex> lock(config_map_mt_);
    return config_map_[key];
}

void ApolloClientSingleNs::setCallback(const ApolloClientSingleNs::Callback &cb)
{
    call_back_ = cb;
}

void ApolloClientSingleNs::setCallback(ApolloClientSingleNs::Callback &&cb)
{
    call_back_ = std::move(cb);
}
void ApolloClientSingleNs::turnOnCallback()
{
    b_call_back = true;
}
void ApolloClientSingleNs::turnOffCallback()
{
    b_call_back = false;
}
