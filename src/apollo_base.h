#ifndef __APOLLO_BASE_H__
#define __APOLLO_BASE_H__

#include <string>
#include <sstream>
#include <map>
#include <set>
#include <yaml-cpp/yaml.h>
#include <tinyxml2.h>
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

// 递归解析yaml节点

void parseYamlNode(const YAML::Node &node, std::map<std::string, std::string> &result, const std::string &prefix = "");

void parseYamlNode(const YAML::Node &node, std::map<std::string, YAML::Node> &result, const std::string &prefix = "");

void parseXmlNode(tinyxml2::XMLElement *node, std::map<std::string, std::string> &result, const std::string &prefix = "");

void parseXmlNode(tinyxml2::XMLElement *node, std::map<std::string, YAML::Node> &result, const std::string &prefix = "");

inline std::string get_non_buffer_url(const std::string &config_server_url,
                                      const std::string &appid,
                                      const std::string &cluster_name,
                                      const std::string &namespace_name)
{
    std::ostringstream ss;
    ss << config_server_url
       << "/configs/"
       << appid << "/" << cluster_name << "/" << namespace_name;
    SPDLOG_INFO("url = {}", ss.str());
    return ss.str();
}
//sprintf(url, "%s/notifications/v2", env_.address_.c_str());
inline std::string get_notify_url(const std::string &config_server_url)
{
    return {config_server_url + (std::string("/notifications/v2"))};
}

template <typename T>
class BasePolicy
{
public:
    bool checkNamespace(const std::string &ns)
    {
        return static_cast<T *>(this)->checkNamespace_(ns);
    }
    bool checkNamespace_(const std::string &ns) { return true; }

    std::optional<std::string> getCf(const std::string &base_url)
    {
        return static_cast<T *>(this)->getCf_(base_url);
    }

    std::optional<std::string> getCf_(const std::string &base_url)
    {
        try {
            auto requestClient = web::http::client::http_client(base_url);
            SPDLOG_INFO("base_url = {}", base_url.c_str());
            auto response = requestClient.request(web::http::methods::GET).get();
            if (response.status_code() == web::http::status_codes::OK) {
                auto json_data_from_server = response.extract_json().get();
                YAML::Node node = YAML::Load(json_data_from_server[U("configurations")].serialize().c_str());
                if (node["content"]) {
                    return node["content"].as<std::string>();
                } else {
                    SPDLOG_ERROR("config content not exist!");
                    return {};
                }
            }
        } catch (std::exception &e) {
            SPDLOG_ERROR("Exception: {}", e.what());
        }
        return {};
    }
    virtual ~BasePolicy() = default;
};

class YamlPolicy : public BasePolicy<YamlPolicy>
{
public:
    static bool checkNamespace_(const std::string &ns)
    {
        return ends_with(ns, std::string(".yaml"));
    }
};

class XmlPolicy : public BasePolicy<XmlPolicy>
{
public:
    static bool checkNamespace_(const std::string &ns)
    {
        return ends_with(ns, std::string(".xml"));
    }
};

class YmlPolicy : public BasePolicy<YamlPolicy>
{
public:
    static bool checkNamespace_(const std::string &ns)
    {
        return ends_with(ns, std::string(".yml"));
    }
};

class PropertiesPolicy : public BasePolicy<PropertiesPolicy>
{
public:
    static bool checkNamespace_(const std::string &ns)
    {
        return true;
    }

    std::optional<std::string> getCf_(const std::string &base_url)
    {
        std::map<std::string, std::string> resMap;
        try {
            auto requestClient = web::http::client::http_client(base_url);
            auto response = requestClient.request(web::http::methods::GET).get();
            if (response.status_code() == web::http::status_codes::OK) {
                // return a json object
                auto jsonData = response.extract_json().get();
                SPDLOG_INFO("jsonData content {}", jsonData.serialize().c_str());
                //get configurations from json object
                //                auto configJsonObj = jsonData[U("configurations")].serialize();
                if (jsonData[U("configurations")].is_object()) {
                    return jsonData[U("configurations")].serialize();
                } else {
                    SPDLOG_INFO("properties is not exist!");
                    return {};
                }
            }
        } catch (std::exception &e) {
            SPDLOG_ERROR("Exception: {}", e.what());
        }
        return {};
    }
};

class JsonPolicy : public BasePolicy<JsonPolicy>
{
public:
    static bool checkNamespace_(const std::string &ns)
    {
        return ends_with(ns, std::string(".json"));
    }
};

class TxtPolicy : public BasePolicy<TxtPolicy>
{
public:
    static bool checkNamespace_(const std::string &ns)
    {
        return ends_with(ns, std::string(".txt"));
    }
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
    static std::map<std::string, std::string> getConfigNoBufferInner(const std::string &config_server_url,
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
    static bool getConfigNoBufferInner(const std::string &config_server_url,
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
    static std::string getConfigNoBufferByKeyInner(const std::string &config_server_url,
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
    template <typename ConfigPolicy = YamlPolicy>
    static std::optional<std::string> getConfig(const std::string &config_server_url,
                                                const std::string &appid_name,
                                                const std::string &namespace_name,
                                                const std::string &cluster_name)
    {
        if ((config_server_url.size() + appid_name.size() + namespace_name.size() + cluster_name.size()) > 266) {
            SPDLOG_ERROR("URL length error!");
            return {};
        }
        std::shared_ptr<BasePolicy<ConfigPolicy>> sp = std::make_shared<ConfigPolicy>();
        if (!sp->checkNamespace(namespace_name)) {
            return {};
        }
        auto base_url = get_non_buffer_url(config_server_url, appid_name, cluster_name, namespace_name);
        return sp->getCf(base_url);
    }

    static std::optional<YAML::Node> getYamlConfig(const std::string &config_server_url,
                                                   const std::string &appid_name,
                                                   const std::string &namespace_name,
                                                   const std::string &cluster_name)
    {
        auto str = getConfig<YamlPolicy>(config_server_url, appid_name, namespace_name, cluster_name);
        if (str.has_value()) {
            auto node = YAML::Load(str.value());
            return node;
        } else {
            return {};
        }
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
