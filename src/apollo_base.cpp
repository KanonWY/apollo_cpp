#include <cpprest/http_client.h>
#include "const_url_value.h"
#include "apollo_base.h"

std::map<std::string, std::string> apollo_base::getConfigNoBufferInner(const std::string &config_server_url,
                                                                  const std::string &appidName,
                                                                  const std::string &namespaceName,
                                                                  const std::string &clusterName)
{
    if ((config_server_url.size() + appidName.size() + namespaceName.size() + clusterName.size()) > 200) {
        std::cout << "url large than 200";
        return {};
    }
    // allocator a return value
    std::map<std::string, std::string> resMap;
    // contact url
    std::string baseUrl;
    baseUrl.reserve(200);
    {
        char *url = new char[200];
        sprintf(url, NO_BUFFER_URL, config_server_url.c_str(),
                appidName.c_str(), clusterName.c_str(),
                namespaceName.c_str());
        baseUrl = url;
        delete[] url;
    }
    try {
        auto requestClient = web::http::client::http_client(baseUrl);
        auto response = requestClient.request(web::http::methods::GET).get();
        if (response.status_code() == web::http::status_codes::OK) {
            // return a json object
            auto jsonData = response.extract_json().get();
            //get configurations from json object
            auto configJsonObj = jsonData[U(CONFIGURATIONS_NAME)].as_object();
            // fill to map
            for (auto &configItem : configJsonObj) {
                resMap.insert({configItem.first, configItem.second.as_string()});
            }
            return resMap;
        }
    } catch (std::exception &e) {
        std::cout << "Exception: " << e.what() << std::endl;
        return resMap;
    }
    return resMap;
}

bool apollo_base::getConfigNoBufferInner(const std::string &config_server_url, const std::string &appidName, const std::string &namespaceName, const std::string &clusterName, std::map<std::string, std::string> &output)
{
    if ((config_server_url.size() + appidName.size() + namespaceName.size() + clusterName.size()) > 200) {
        std::cout << "url large than 200" << std::endl;
        return false;
    }
    // allocator a return value
    std::map<std::string, std::string> resMap;
    // contact url
    std::string baseUrl;
    baseUrl.reserve(200);
    {
        char *url = new char[200];
        sprintf(url, NO_BUFFER_URL, config_server_url.c_str(),
                appidName.c_str(), clusterName.c_str(),
                namespaceName.c_str());
        baseUrl = url;
        delete[] url;
    }
    try {
        auto requestClient = web::http::client::http_client(baseUrl);
        auto response = requestClient.request(web::http::methods::GET).get();
        if (response.status_code() == web::http::status_codes::OK) {
            // return a json object
            auto jsonData = response.extract_json().get();
            //get configurations from json object
            auto configJsonObj = jsonData[U(CONFIGURATIONS_NAME)].as_object();
            // fill to map
            for (auto &configItem : configJsonObj) {
                resMap.insert({configItem.first, configItem.second.as_string()});
            }
            return true;
        }
    } catch (std::exception &e) {
        std::cout << "Exception: " << e.what() << std::endl;
        return false;
    }
    return false;
}



std::string apollo_base::getConfigNoBufferByKeyInner(const std::string &config_server_url, const std::string &appidName, const std::string &namespaceName, const std::string &clusterName, const std::string &key)
{
    if ((config_server_url.size() + appidName.size() + namespaceName.size() + clusterName.size()) > 200) {
        std::cout << "url large than 200";
        return {};
    }
    // contact url
    std::string baseUrl;
    baseUrl.reserve(200);
    {
        char *url = new char[200];
        sprintf(url, NO_BUFFER_URL, config_server_url.c_str(),
                appidName.c_str(), clusterName.c_str(),
                namespaceName.c_str());
        baseUrl = url;
        delete[] url;
    }
    try {
        auto requestClient = web::http::client::http_client(baseUrl);
        auto response = requestClient.request(web::http::methods::GET).get();
        if (response.status_code() == web::http::status_codes::OK) {
            // return a json object
            auto jsonData = response.extract_json().get();
            //get configurations from json object
            auto configJsonObj = jsonData[U(CONFIGURATIONS_NAME)].as_object();
            // fill to map
            for (auto &configItem : configJsonObj) {
                if (!strcmp(configItem.first.c_str(), key.c_str())) {
                    return configItem.second.as_string();
                }
            }
        }
    } catch (std::exception &e) {
        std::cout << "Exception: " << e.what() << std::endl;
        return {};
    }
    return {};
}
