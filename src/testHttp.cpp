#include <iostream>
#include <cpprest/http_client.h>
#include "apollo_basic.h"

/**
 *      @brief
 *
 *      本文件仅作接口测试
 * */

/**
 *
 *     @brief  带缓存的http接口，从Apollo读取配置
 *
 *          该接口会从缓存中获取配置，适合频率较高的配置拉取请求，如简单的每30秒轮询一次配置
 *          不是从db中直接获取的，
 *
 *      URL:
 * {config_server_url}/configfiles/json/{appId}/{clusterName}/{namespaceName}?ip={clientIp}
 * **/

const char* BUFFER_URL = "%s/configfiles/json/%s/%s/%s";

//基础接口
std::string getKeyValueBuffer(const std::string& key,
                              const std::string& serverAddress,
                              const std::string& appId,
                              const std::string& clusterName = "default",
                              const std::string& namespaceName = "application")
{
    // check
    if ((key.size() + serverAddress.size() + appId.size() + clusterName.size() +
         namespaceName.size()) > 200)
    {
        return {};
    }
    // 1. contact url
    char urlStack[200] = {0};
    sprintf(urlStack, BUFFER_URL, serverAddress.c_str(), appId.c_str(),
            clusterName.c_str(), namespaceName.c_str());
    std::string url(urlStack);

    std::cout << "url = " << url << std::endl;

    // 2. apply request
    auto requestJson = web::http::client::http_client(url);
    try
    {
        auto response = requestJson.request(web::http::methods::GET).get();
        if (response.status_code() == web::http::status_codes::OK)
        {
            auto jsonData = response.extract_json().get();
            auto jsObj = jsonData.as_object();
            for (auto it = jsObj.cbegin(); it != jsObj.end(); ++it)
            {
                if (!strcmp(it->first.c_str(), key.c_str()))
                {
                    return it->second.as_string();
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
    return {};
}

/**
 *
 *
 *
 *      URL:
 * {config_server_url}/configs/{appId}/{clusterName}/{namespaceName}?releaseKey={releaseKey}&messages={messages}&label={label}&ip={clientIp}
 *
 *
 *
 *
 *          示例：
 *
 *
 *
 *
 * */

//
// put 请求：http://localhost:8080/configs/codereview/default/codereviwNS
//
// josn 返回格式
//{
//    "appId": "codereview",
//    "cluster": "default",
//    "namespaceName": "codereviwNS",
//    "configurations": {
//              "CodeBuild": "git clone xxx && cd xxx && make -j",
//               "debug": "YES",
//               "testmodify": "value2",
//               "Numbertest": "222",
//               "testsjon": "{\n   \"one\": \"yyey\"\n}"
//     },
//    "releaseKey": "20230323130108-c88c7ef99febead7"
//}
//
//
//const char* NO_BUFFER_URL = "%s/configs/%s/%s/%s";
//const char* CONFIGURATIONS_NAME = "configurations";

//基础接口
std::string getKeyValueNoBuffer(
    const std::string& key, const std::string& serverAddress,
    const std::string& appId, const std::string& clusterName = "default",
    const std::string& namespaceName = "application")
{
    // check
    if ((key.size() + serverAddress.size() + appId.size() + clusterName.size() +
         namespaceName.size()) > 200)
    {
        return {};
    }
    // 1. contact url
    char urlStack[200] = {0};
    sprintf(urlStack, NO_BUFFER_URL, serverAddress.c_str(), appId.c_str(),
            clusterName.c_str(), namespaceName.c_str());
    std::string url(urlStack);

    std::cout << "url = " << url << std::endl;

    // 2. apply request
    auto requestJson = web::http::client::http_client(url);
    try
    {
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
    return {};
}

std::map<std::string, std::string> getAllKeyValueMap(const std::string& url)
{
    std::map<std::string, std::string> m_map;
    auto requestJson = web::http::client::http_client(url);
    try
    {
        auto res = requestJson.request(web::http::methods::GET).get();
        if (res.status_code() == web::http::status_codes::OK)
        {
            std::cout << "receive ok" << std::endl;
            auto jsonData = res.extract_json().get();
            auto jsObj = jsonData.as_object();
            for (auto it = jsObj.cbegin(); it != jsObj.end(); ++it)
            {
                m_map.insert({it->first, it->second.as_string()});
            }
        }
        return m_map;
    }
    catch (std::exception& e)
    {
        std::cout << "Exception: " << e.what() << std::endl;
        return m_map;
    }
}

/**
 *  @brief 应用感知更新
 *
 * */

std::map<std::string, uint64_t> NotifyMap;
std::atomic<bool> g_longPoll_;

struct NotifyMix
{
    std::string Namespace;
    int notify = -1;
};

void RemoteConfigLongPollService(const NotifyMix& notify)
{
    Properties oldProperties;
}

void

    int
    testgetKeyValueBuffer()
{
    auto value = getKeyValueBuffer("testmodify", "http://localhost:8080",
                                   "codereview", "default", "codereviwNS");
    if (!value.empty())
    {
        std::cout << "value = " << value.c_str() << std::endl;
    }
    return 0;
}

int testgetKeyValueNoBuffer()
{
    auto value = getKeyValueNoBuffer("testmodify", "http://localhost:8080",
                                     "codereview", "default", "codereviwNS");
    if (!value.empty())
    {
        std::cout << "value = " << value.c_str() << std::endl;
    }
    return 0;
}
