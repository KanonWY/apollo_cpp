#include "apollo_basic.h"
#include <iostream>
#include <cpprest/http_client.h>

std::string getNoCachePropertyString(const apolloEnv& env,
                                     const std::string& key)
{
    return std::string{};
}

Properties getNoCacheProperty(const apolloEnv& env)
{
    Properties sp;

    return sp;
}

Properties ApolloClient::getPropertiesNoBuffer()
{
    Properties res;
    char urlStack[200] = {0};
    sprintf(urlStack, NO_BUFFER_URL, nev_param_.address_.c_str(),
            nev_param_.appid_name_.c_str(), nev_param_.cluster_name_.c_str(),
            nev_param_.namespace_name_.c_str());
    std::string url(urlStack);

    std::cout << "url = " << url << std::endl;

    try
    {
        auto requestJson = web::http::client::http_client(url);
        auto response = requestJson.request(web::http::methods::GET).get();
        if (response.status_code() == web::http::status_codes::OK)
        {
            auto jsonData = response.extract_json().get();
            auto jsObj = jsonData.as_object();
            for (auto it = jsObj.cbegin(); it != jsObj.end(); ++it)
            {
                // get configurations json object
                if (!strcmp(it->first.c_str(), CONFIGURATIONS_NAME))
                {
                    auto configJsonData = it->second.as_object();
                    for (auto configItem = configJsonData.cbegin();
                         configItem != configJsonData.end(); ++configItem)
                    {
                        res.kvmap_.insert({configItem->first,
                                           configItem->second.as_string()});
                    }
                }
            }
            return res;
        }
        else
        {
            return res;
        }
    }
    catch (std::exception& e)
    {
        std::cout << "Exception: " << e.what() << std::endl;
        return res;
    }
}

std::string ApolloClient::getValueNoBuffer(const std::string& key)
{
    char urlStack[200] = {0};
    sprintf(urlStack, NO_BUFFER_URL, nev_param_.address_.c_str(),
            nev_param_.appid_name_.c_str(), nev_param_.cluster_name_.c_str(),
            nev_param_.namespace_name_.c_str());
    std::string url(urlStack);

    std::cout << "url = " << url << std::endl;

    try
    {
        auto requestJson = web::http::client::http_client(url);
        auto response = requestJson.request(web::http::methods::GET).get();
        if (response.status_code() == web::http::status_codes::OK)
        {
            auto jsonData = response.extract_json().get();
            auto jsObj = jsonData.as_object();
            for (auto it = jsObj.cbegin(); it != jsObj.end(); ++it)
            {
                // get configurations json object
                if (!strcmp(it->first.c_str(), CONFIGURATIONS_NAME))
                {
                    auto configJsonData = it->second.as_object();
                    for (auto configItem = configJsonData.cbegin();
                         configItem != configJsonData.end(); ++configItem)
                    {
                        if (!strcmp(configItem->first.c_str(), key.c_str()))
                        {
                            return configItem->second.as_string();
                        }
                    }
                }
            }
        }
        else
        {
            return {};
        }
    }
    catch (std::exception& e)
    {
        std::cout << "Exception: " << e.what() << std::endl;
        return {};
    }
    return std::string();
}
void ApolloClient::startLongPoll() {}

void ApolloClient::stopLongPoll()
{
    b_long_poll_ = false;
    if (long_poll_thread_.joinable())
    {
        long_poll_thread_.join();
    }
}
void ApolloClient::longPollFunc() {}

void ApolloClient::checkNotification()
{
    // contact the url
    char stackUrl[200] = {0};
    sprintf(stackUrl, NOTIFICATIONS_URL, nev_param_.address_.c_str(),
            nev_param_.cluster_name_.c_str(), name_nid_map_.begin()->second);
    std::string url(stackUrl);
    std::cout << "CheckUrl = " << url.c_str() << std::endl;
    // create http request and judge the res for status
    try
    {
        auto requestJson = web::http::client::http_client(url);

        // make namespace and notifiedid into json and append_query
        web::http::uri_builder builder;
        web::json::value notifications;
        for(auto item : name_nid_map_)
        {
            notifications[U(item.first.c_str())] = web::json::value(item.second);
        }
        builder.append_query(U("notifications"),notifications);

        auto response = requestJson.request(web::http::methods::GET, builder.to_string()).get();

        // check the response and wrap as a package return.
        if (response.status_code() == web::http::status_codes::OK)
        {

        }
    }
    catch (std::exception& e)
    {
        std::cout << "Exception: " << e.what() << std::endl;
    }
}
