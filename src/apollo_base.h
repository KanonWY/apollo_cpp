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

template <typename charT>
inline bool starts_with(const std::basic_string<charT> &big, const std::basic_string<charT> &small)
{
    if (&big == &small)
        return true;
    const typename std::basic_string<charT>::size_type big_size = big.size();
    const typename std::basic_string<charT>::size_type small_size = small.size();
    const bool valid_ = (big_size >= small_size);
    const bool starts_with_ = (big.compare(0, small_size, small) == 0);
    return valid_ and starts_with_;
}

template <typename charT>
inline bool ends_with(const std::basic_string<charT> &big, const std::basic_string<charT> &small)
{
    if (&big == &small)
        return true;
    const typename std::basic_string<charT>::size_type big_size = big.size();
    const typename std::basic_string<charT>::size_type small_size = small.size();
    const bool valid_ = (big_size >= small_size);
    const bool ends_with_ = (big.compare(big_size - small_size, small_size, small) == 0);
    return valid_ and ends_with_;
}

enum RE_TYPE
{
    YAML_OBJECT,
    YAML_STRING,
    STRING_MAP,
};

class apollo_base
{
public:
    /**
     * @brief get config from db.(not config server buffer)
     * @param config_server_url
     * @param appid_name
     * @param namespace_name
     * @param cluster_name
     * @return
     */
    std::map<std::string, std::string> getConfigNoBufferInner(const std::string &config_server_url,
                                                              const std::string &appid_name,
                                                              const std::string &namespace_name,
                                                              const std::string &cluster_name);

    /**
     * @brief get config from db.(not config server buffer)
     * @param config_server_url
     * @param appid_name
     * @param namespace_name
     * @param cluster_name
     * @param output
     * @return
     */
    bool getConfigNoBufferInner(const std::string &config_server_url,
                                const std::string &appid_name,
                                const std::string &namespace_name,
                                const std::string &cluster_name,
                                std::map<std::string, std::string> &output);

    /**
     * @brief get config from db by key.
     * @param config_server_url
     * @param appid_name
     * @param namespace_name
     * @param cluster_name
     * @param key
     * @return
     */
    std::string getConfigNoBufferByKeyInner(const std::string &config_server_url,
                                            const std::string &appid_name,
                                            const std::string &namespace_name,
                                            const std::string &cluster_name,
                                            const std::string &key);

    /**
     * @brief  get config from db, return yaml first level key:value string.
     * @param config_server_url
     * @param appid_name
     * @param namespace_name
     * @param cluster_name
     * @return
     */
    static std::map<std::string, std::string> getConfigNoBufferInnerByYAML(const std::string &config_server_url,
                                                                           const std::string &appid_name,
                                                                           const std::string &namespace_name,
                                                                           const std::string &cluster_name);

public:
    /**
     * @brief get yaml node for db.
     * @tparam T value template.
     * @param config_server_url
     * @param appid_name
     * @param namespace_name
     * @param cluster_name
     * @return
     */
    template <RE_TYPE T>
    typename std::enable_if<T == RE_TYPE::YAML_OBJECT, YAML::Node>::type getYamlConfig(const std::string &config_server_url,
                                                                                       const std::string &appid_name,
                                                                                       const std::string &namespace_name,
                                                                                       const std::string &cluster_name)
    {
        YAML::Node node;
        //TODO: check the namespace_name end with .yaml
        if (!ends_with(namespace_name, std::string(".yaml"))) {
            SPDLOG_ERROR("namespace_name error, it must be .yaml!");
            return node;
        }
        if ((config_server_url.size() + appid_name.size() + namespace_name.size() + cluster_name.size()) > 266) {
            SPDLOG_ERROR("URL length error!");
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
            auto response = requestClient.request(web::http::methods::GET).get();
            if (response.status_code() == web::http::status_codes::OK) {
                auto json_data_from_server = response.extract_json().get();
                node = YAML::Load(json_data_from_server[U("configurations")].serialize().c_str());
                if (node["content"]) {
                    return node["content"];
                }
                SPDLOG_ERROR("config content not exist, maybe not yaml config type!");
            }
        } catch (std::exception &e) {
            SPDLOG_ERROR("Exception: {}", e.what());
        }
        return node;
    }

    /**
     * @brief get config string from db.
     * @tparam T
     * @param config_server_url
     * @param appid_name
     * @param namespace_name
     * @param cluster_name
     * @return
     */
    template <RE_TYPE T>
    typename std::enable_if<T == RE_TYPE::YAML_STRING, std::string>::type getYamlConfig(const std::string &config_server_url,
                                                                                        const std::string &appid_name,
                                                                                        const std::string &namespace_name,
                                                                                        const std::string &cluster_name)
    {
        std::string res_value;
        if (!ends_with(namespace_name, std::string(".yaml"))) {
            SPDLOG_ERROR("namespace_name error, it must be .yaml!");
            return res_value;
        }
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
                SPDLOG_ERROR("config content not exist, maybe not yaml config type!");
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

/**
 * @brief basic http interface
 */

class apollo_openapi_base
{
public:
    /**
     * @brief init the default token.
     * @param token
     */
    void init(const std::string &token);

    /**
     * @brief get app env
     *        URL : http://{portal_address}/openapi/v1/apps/{appId}/envclusters
     *        Method : GET
     * @param address
     * @param appid
     * @return
     */
    std::string getAppenvInfo(const std::string &address,
                              const std::string &appid);

    /**
     * @brief get app detail info
     *        URL : http://{portal_address}/openapi/v1/apps
     *        Method : GET
     * @param address
     * @return
     */
    std::string getAppInfo(const std::string &address);

    /**
     * @brief get cluster info (include cluster name, data change time)
     *        URL:  http://{portal_address}/openapi/v1/envs/{env}/apps/{appId}/clusters/{clusterName}
     *        METHOD： GET
     * @param address
     * @param envinfo
     * @param appid
     * @param clustername
     * @return
     */
    std::string getClusterInfo(const std::string &address,
                               const std::string &envinfo,
                               const std::string &appid,
                               const std::string &clustername);

    /**
     * @brief create cluster
     *        URL:  http://{portal_address}/openapi/v1/envs/{env}/apps/{appId}/clusters
     *        Method: POST
     * @param address
     * @param envinfo
     * @param appid
     * @return
     */
    std::string createCluster(const std::string &address,
                              const std::string &envinfo,
                              const std::string &appid);

    /**
     * @brief get all  namespace info in a cluster.
     *        URL: http://{portal_address}/openapi/v1/envs/{env}/apps/{appId}/clusters/{clusterName}/namespaces
     *        Method: GET
     * @param address
     * @param env
     * @param appid
     * @param clustername
     * @return
     */
    std::string getNamespaceinfoInCluster(const std::string &address,
                                          const std::string &env,
                                          const std::string &appid,
                                          const std::string &clustername);

    /**
     * @brief get a namespace info
     *        URL: http://{portal_address}/openapi/v1/envs/{env}/apps/{appId}/clusters/{clusterName}/namespaces/{namespaceName}
     *        Method: GET
     * @param address
     * @param env
     * @param appid
     * @param clustername
     * @param namespacesname
     * @return
     */
    std::string getSpecialNamespaceInfo(const std::string &address,
                                        const std::string &env,
                                        const std::string &appid,
                                        const std::string &clustername,
                                        const std::string &namespacesname);

    /**
     * @brief create a new namespace
     *        URL: http://{portal_address}/openapi/v1/apps/{appId}/appnamespaces
     *        Method: POST
     * @param address
     * @param appid
     * @return
     */
    bool createNewNamespace(const std::string &address,
                            const std::string &appid,
                            const std::string &namespacename,
                            const std::string &format,
                            const std::string &dataChangeCreatedBy,
                            bool iPublic,
                            const std::string &comment);

    /**
     * @brief get config interface
     *        URL: http://{portal_address}/openapi/v1/envs/{env}/apps/{appId}/clusters/{clusterName}/namespaces/{namespaceName}/items/{key}
     *        Method: GET
     * @param address
     * @param env
     * @param appid
     * @param clustername
     * @param namespacename
     * @return
     */
    std::string getConfigNoProperties(const std::string &address,
                                      const std::string &env,
                                      const std::string &appid,
                                      const std::string &clustername,
                                      const std::string &namespacename);

    /**
     * @brief create new config
     * @param address
     * @param env
     * @param appid
     * @param clustername
     * @param namespacename
     * @return
     */
    bool createNewConfig(const std::string &address,
                         const std::string &env,
                         const std::string &appid,
                         const std::string &clustername,
                         const std::string &namespacename,
                         const std::string &key,
                         const std::string &value,
                         const std::string &dataChangeCreatedBy,
                         const std::string &comment);

    /**
     * @brief  modify config
     *         URL: http://{portal_address}/openapi/v1/envs/{env}/apps/{appId}/clusters/{clusterName}/namespaces/{namespaceName}/items
     *         Method: POST
     * @param address
     * @param env
     * @param appid
     * @param clustername
     * @param namespacename
     * @param conifgstr
     * @param modifyuserid
     * @param createIfnotExists
     * @param comment
     * @param createuerid
     * @return
     */
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

    /**
     * @brief  delete config,
     *         URL:  http://{portal_address}/openapi/v1/envs/{env}/apps/{appId}/clusters/{clusterName}/namespaces/{namespaceName}/items/{key}?
     *         Method: DELETE
     * @param address
     * @param env
     * @param appid
     * @param clustername
     * @param namespacename
     * @param deleteuserid
     * @return
     */
    bool deleteConfig(const std::string &address,
                      const std::string &env,
                      const std::string &appid,
                      const std::string &clustername,
                      const std::string &namespacename,
                      const std::string &deleteuserid);

    /**
     * @brief publish config: Method： POST
     * @param address  URL: http://{portal_address}/openapi/v1/envs/{env}/apps/{appId}/clusters/{clusterName}/namespaces/{namespaceName}/releases
     * @param env
     * @param appid
     * @param clusterName
     * @param namespacename
     * @param releaseTitle
     * @param releasedBy
     * @param releaseComment
     * @return
     */
    bool publishConfig(const std::string &address,
                       const std::string &env,
                       const std::string &appid,
                       const std::string &clusterName,
                       const std::string &namespacename,
                       const std::string &releaseTitle,
                       const std::string &releasedBy,
                       const std::string &releaseComment);

    /**
     * @brief rollback the config, URL:  http://{portal_address}/openapi/v1/envs/{env}/releases/{releaseId}/rollback
     * @param address
     * @param env
     * @param releaseid rollback to which version.
     */
    void rollbackConfig(const std::string &address,
                        const std::string &env,
                        const std::string &releaseid);

    /**
     * @brief get a headers which container given token.
     * @param token
     * @return
     */
    static web::http::http_headers getTokenHeader(const std::string &token);

private:
    std::string token_{};
};

} // namespace apollo_client

#endif
