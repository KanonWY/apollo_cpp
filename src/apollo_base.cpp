#include <cpprest/http_client.h>
#include "const_url_value.h"
#include "apollo_base.h"
#include <spdlog/spdlog.h>

namespace apollo_client
{

std::map<std::string, std::string> apollo_base::getConfigNoBufferInner(const std::string &config_server_url,
                                                                       const std::string &appidName,
                                                                       const std::string &namespaceName,
                                                                       const std::string &clusterName)
{
    if ((config_server_url.size() + appidName.size() + namespaceName.size() + clusterName.size()) > 200) {
        SPDLOG_ERROR("url large than 200");
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
            SPDLOG_INFO("jsonData content {}",jsonData.serialize().c_str());
            //get configurations from json object
            auto configJsonObj = jsonData[U(CONFIGURATIONS_NAME)].as_object();
            // fill to map
            for (auto &configItem : configJsonObj) {
                resMap.insert({configItem.first, configItem.second.as_string()});
            }
            return resMap;
        }
    } catch (std::exception &e) {
        SPDLOG_ERROR("Exception: {}", e.what());
        return resMap;
    }
    return resMap;
}

bool apollo_base::getConfigNoBufferInner(const std::string &config_server_url, const std::string &appidName, const std::string &namespaceName, const std::string &clusterName, std::map<std::string, std::string> &output)
{
    if ((config_server_url.size() + appidName.size() + namespaceName.size() + clusterName.size()) > 200) {
        SPDLOG_ERROR("url larger than 200!");
        return false;
    }
    // contact url
    output.clear();
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
            SPDLOG_INFO("requestClient => {}", baseUrl.c_str());
            // return a json object
            auto jsonData = response.extract_json().get();
            //get configurations from json object
            auto configJsonObj = jsonData[U(CONFIGURATIONS_NAME)].as_object();
            // fill to map
            for (auto &configItem : configJsonObj) {
                output.insert({configItem.first, configItem.second.as_string()});
            }
            return true;
        }
    } catch (std::exception &e) {
        SPDLOG_ERROR("Exception: {}", e.what());
        return false;
    }
    return false;
}

std::string apollo_base::getConfigNoBufferByKeyInner(const std::string &config_server_url, const std::string &appidName, const std::string &namespaceName, const std::string &clusterName, const std::string &key)
{
    if ((config_server_url.size() + appidName.size() + namespaceName.size() + clusterName.size()) > 200) {
        SPDLOG_ERROR("url larger than 200!");
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
        SPDLOG_ERROR("Exception: {}", e.what());
        return {};
    }
    return {};
}
std::map<std::string, std::string> apollo_base::getConfigNoBufferInnerByYAML(const std::string &config_server_url, const std::string &appid_name, const std::string &namespace_name, const std::string &cluster_name)
{
    if ((config_server_url.size() + appid_name.size() + namespace_name.size() + cluster_name.size()) > 200) {
        SPDLOG_ERROR("url large than 200");
        return {};
    }
    // allocator a return value
    std::map<std::string, std::string> resMap;
    // contact url
    std::string baseUrl;
    std::string namespace_filename = namespace_name;
    namespace_filename.append(".yaml");
    baseUrl.reserve(200);
    {
        char *url = new char[200];
        sprintf(url, NO_BUFFER_URL, config_server_url.c_str(),
                appid_name.c_str(), cluster_name.c_str(),
                namespace_filename.c_str());
        baseUrl = url;
        delete[] url;
    }
    try {
        auto requestClient = web::http::client::http_client(baseUrl);
        SPDLOG_INFO("base_url = {}", baseUrl.c_str());
        auto response = requestClient.request(web::http::methods::GET).get();
        if (response.status_code() == web::http::status_codes::OK) {
            // return a json object
            auto jsonData = response.extract_json().get();
            SPDLOG_INFO("jsonData type = {}", jsonData.type());
            SPDLOG_INFO("jsonData content = {}", jsonData.serialize().c_str());
            //get configurations from json object
            auto configJsonObj = jsonData[U(CONFIGURATIONS_NAME)].as_object();
            // fill to map
            for (auto &configItem : configJsonObj) {
                resMap.insert({configItem.first, configItem.second.as_string()});
            }
            return resMap;
        }
    } catch (std::exception &e) {
        SPDLOG_ERROR("Exception: {}", e.what());
        return resMap;
    }
    return resMap;
}

} // namespace apollo_client