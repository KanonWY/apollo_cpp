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

    // 1.get app env
    // URL : http://{portal_address}/openapi/v1/apps/{appId}/envclusters
    // Method : GET
    std::string getAppenvInfo(const std::string &address,
                              const std::string &appid);

    // 2.get app detail info
    // URL : http://{portal_address}/openapi/v1/apps
    // Method : GET
    std::string getAppInfo(const std::string &address);

    // 3. get cluster info (include cluster name, data change time)
    // URL:  http://{portal_address}/openapi/v1/envs/{env}/apps/{appId}/clusters/{clusterName}
    // METHOD： GET
    std::string getClusterInfo(const std::string &address,
                               const std::string &envinfo,
                               const std::string &appid,
                               const std::string &clustername);

    // 4. create cluster
    // URL:  http://{portal_address}/openapi/v1/envs/{env}/apps/{appId}/clusters
    // Method: POST
    std::string createCluster(const std::string &address,
                              const std::string &envinfo,
                              const std::string &appid);

    // 5. get all  namespace info in a cluster.
    // URL: http://{portal_address}/openapi/v1/envs/{env}/apps/{appId}/clusters/{clusterName}/namespaces
    // Method: GET
    std::string getNamespaceinfoInCluster(const std::string &address,
                                          const std::string &env,
                                          const std::string &appid,
                                          const std::string &clustername);

    // 6. get a namespace info
    // URL: http://{portal_address}/openapi/v1/envs/{env}/apps/{appId}/clusters/{clusterName}/namespaces/{namespaceName}
    // Method: GET
    std::string getSpecialNamespaceInfo(const std::string &address,
                                        const std::string &env,
                                        const std::string &appid,
                                        const std::string &clustername,
                                        const std::string &namespacesname);

    // 7. create a new namespace.
    // URL: http://{portal_address}/openapi/v1/apps/{appId}/appnamespaces
    // POST
    std::string createNewNamespace(const std::string &address,
                                   const std::string &appid);

    // 8. locker interface TODO

    //9. get config interface
    // URL: http://{portal_address}/openapi/v1/envs/{env}/apps/{appId}/clusters/{clusterName}/namespaces/{namespaceName}/items/{key}
    // Method: GET
    std::optional<std::string> getConifgNoProperties(const std::string &address,
                                                     const std::string &env,
                                                     const std::string &appid,
                                                     const std::string &clustername,
                                                     const std::string &namespacename);

    // 10. create new config
    // URL: http://{portal_address}/openapi/v1/envs/{env}/apps/{appId}/clusters/{clusterName}/namespaces/{namespaceName}/items
    // Method: POST
    std::string createNewconfig(const std::string &address,
                                const std::string &env,
                                const std::string &appid,
                                const std::string &clustername,
                                const std::string &namespacename);

    // 11. modify config
    // URL: http://{portal_address}/openapi/v1/envs/{env}/apps/{appId}/clusters/{clusterName}/namespaces/{namespaceName}/items/{key}
    // Method: GET
    // DESC: if key == "content", the config file is not properties.
    bool modifyConfigNoProperties(const std::string &address,
                                  const std::string &env,
                                  const std::string &appid,
                                  const std::string &clustername,
                                  const std::string &namespacename,
                                  const std::string &conifgstr,
                                  const std::string &modifyuserid,
                                  bool createIfnotExists,
                                  const std::string &comment,
                                  const std::string &createuerid);

    // 12. delete config
    // URL:  http://{portal_address}/openapi/v1/envs/{env}/apps/{appId}/clusters/{clusterName}/namespaces/{namespaceName}/items/{key}?
    // Method: DELETE
    bool deleteConfig(const std::string &address,
                      const std::string &env,
                      const std::string &appid,
                      const std::string &clustername,
                      const std::string &namespacename,
                      const std::string &deleteuserid);

    // 13. publish config
    // URL: http://{portal_address}/openapi/v1/envs/{env}/apps/{appId}/clusters/{clusterName}/namespaces/{namespaceName}/releases
    // Method： POST
    bool publishConfig(const std::string &address,
                       const std::string &env,
                       const std::string &appid,
                       const std::string &clusterName,
                       const std::string &namespacename,
                       const std::string &releaseTitle,
                       const std::string &releasedBy,
                       const std::string &releaseComment);

    // 14 rollback the config
    // URL:  http://{portal_address}/openapi/v1/envs/{env}/releases/{releaseId}/rollback
    // Method： PUT
    // releaseid: rollback to which version.
    void rollbackConfig(const std::string &address,
                        const std::string &env,
                        const std::string &releaseid);
};

} // namespace apollo_client

#endif
