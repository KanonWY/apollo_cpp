#ifndef __APOLLO_BASE_H__
#define __APOLLO_BASE_H__

#include <string>
#include <map>
#include <set>
#include <yaml-cpp/yaml.h>
#include <spdlog/spdlog.h>
#include <cpprest/http_client.h>

namespace apollo_client
{

enum RE_TYPE
{
    YAML_OBJECT,
    YAML_STRING,
    STRING_MAP,
};

class apollo_base
{
public:
    std::map<std::string, std::string> getConfigNoBufferInner(const std::string &config_server_url,
                                                              const std::string &appid_name,
                                                              const std::string &namespace_name,
                                                              const std::string &cluster_name);

    bool getConfigNoBufferInner(const std::string &config_server_url,
                                const std::string &appid_name,
                                const std::string &namespace_name,
                                const std::string &cluster_name,
                                std::map<std::string, std::string> &output);

    std::string getConfigNoBufferByKeyInner(const std::string &config_server_url,
                                            const std::string &appid_name,
                                            const std::string &namespace_name,
                                            const std::string &cluster_name,
                                            const std::string &key);

    std::map<std::string, std::string> getConfigNoBufferInnerByYAML(const std::string &config_server_url,
                                                                    const std::string &appid_name,
                                                                    const std::string &namespace_name,
                                                                    const std::string &cluster_name);

public:
    template <RE_TYPE T>
    typename std::enable_if<T == RE_TYPE::YAML_OBJECT, YAML::Node>::type getYamlConfig(const std::string &config_server_url,
                                                                                       const std::string &appid_name,
                                                                                       const std::string &namespace_name,
                                                                                       const std::string &cluster_name)
    {
        YAML::Node node;
        //TODO: check the namespace_name end with .yaml
        if ((config_server_url.size() + appid_name.size() + namespace_name.size() + cluster_name.size()) > 266) {
            SPDLOG_ERROR("url large than 200");
            return node;
        }
        std::string base_url;
        base_url.reserve(266);
        {
            char *url = new char[266];
            sprintf(url, "%s/configs/%s/%s/%s", config_server_url.c_str(),
                    appid_name.c_str(), cluster_name.c_str(),
                    namespace_name.c_str());
            base_url = url;
            delete[] url;
        }
        try {
            auto requestClient = web::http::client::http_client(base_url);
            SPDLOG_INFO("base_url = {}", base_url.c_str());
            auto response = requestClient.request(web::http::methods::GET).get();
            if (response.status_code() == web::http::status_codes::OK) {
                auto json_data_from_server = response.extract_json().get();
                node = YAML::Load(json_data_from_server[U("configurations")].serialize().c_str());
                if (node["content"]) {
                    return node["content"];
                }
                SPDLOG_ERROR("config content not exist!");
            }
        } catch (std::exception &e) {
            SPDLOG_ERROR("Exception: {}", e.what());
        }
        return node;
    }

    template <RE_TYPE T>
    typename std::enable_if<T == RE_TYPE::YAML_STRING, std::string>::type getYamlConfig(const std::string &config_server_url,
                                                                                        const std::string &appid_name,
                                                                                        const std::string &namespace_name,
                                                                                        const std::string &cluster_name)
    {
        std::string res_value;
        if ((config_server_url.size() + appid_name.size() + namespace_name.size() + cluster_name.size()) > 266) {
            SPDLOG_ERROR("url large than 200");
            return res_value;
        }
        std::string base_url;
        base_url.reserve(266);
        {
            char *url = new char[266];
            sprintf(url, "%s/configs/%s/%s/%s", config_server_url.c_str(),
                    appid_name.c_str(), cluster_name.c_str(),
                    namespace_name.c_str());
            base_url = url;
            delete[] url;
        }
        try {
            auto requestClient = web::http::client::http_client(base_url);
            SPDLOG_INFO("base_url = {}", base_url.c_str());
            auto response = requestClient.request(web::http::methods::GET).get();
            if (response.status_code() == web::http::status_codes::OK) {
                auto json_data_from_server = response.extract_json().get();
                YAML::Node node = YAML::Load(json_data_from_server[U("configurations")].serialize().c_str());
                if (node["content"]) {
                    res_value = node["content"].as<std::string>();
                }
            }
        } catch (std::exception &e) {
            SPDLOG_ERROR("Exception: {}", e.what());
        }
        return res_value;
    }

    template <RE_TYPE T>
    typename std::enable_if<T == RE_TYPE::STRING_MAP, std::map<std::string, std::string>>::type getYamlConfig(const std::string &config_server_url,
                                                                                                              const std::string &appid_name,
                                                                                                              const std::string &namespace_name,
                                                                                                              const std::string &cluster_name)
    {
        //TODO
        return std::map<std::string, std::string>{};
    }

    virtual ~apollo_base() = default;
};
//This is can change the server config

class apollo_openapi_base
{
public:
    std::set<std::map<std::string, std::string>> getAllAppInfo(const std::string &appIds);

    // cluster about

    // 1. get cluster info

    // 2. create a new cluster

    // ns about

    // config about

    // 1. get config

    // 2. add config

    // 3. modify config

    // 4. delete config

    // 5. pub config
};

} // namespace apollo_client

#endif