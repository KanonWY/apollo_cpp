#ifndef APOLLO_CPP_CLIENT_APOLLO_OPENAPI_BASE_H
#define APOLLO_CPP_CLIENT_APOLLO_OPENAPI_BASE_H

#include <string>
#include <map>
#include <vector>
#include <yaml-cpp/yaml.h>
#include <cpprest/http_client.h>
#include <spdlog/spdlog.h>
#include "apollo_base_client.h"
#include "yaml_code_template.h"

namespace apollo_client
{

/**
 * @brief for TagDispatchTrait
 */

enum REQUEST_TYPE
{
    GET_APP_INFO,
    GET_CLUSTER_NS_INFO,
};

struct GET_APP_INFO_Tag
{
};
struct GET_CLUSTER_NS_INFO_Tag
{
};

template <REQUEST_TYPE T>
struct TagDispatchTrait
{
};

template <>
struct TagDispatchTrait<GET_APP_INFO>
{
    using Tag = GET_APP_INFO_Tag;
};

template <>
struct TagDispatchTrait<GET_CLUSTER_NS_INFO>
{
    using Tag = GET_CLUSTER_NS_INFO_Tag;
};

template <typename T>
struct is_NodeChangeAble : std::integral_constant<bool, std::is_integral_v<T> || std::is_floating_point_v<T>>
{
};

template <typename T>
inline constexpr bool is_NonChangeAble_v = is_NodeChangeAble<T>::value;

/**
 * @brief apollo_ctrl config
 */
class apollo_ctrl_base
{
public:
    void init(const std::string &token, const apollo_client::MultiNsConfig &config)
    {
        config_ = config;
        token_ = token;
        // get all config store into map.
        updateAllNamespaceInfoInThisAppid();
    }

private:
    ///////////////////////////////////////////////////////////////////////////
    template <typename... Types, REQUEST_TYPE T>
    web::http::http_request internal_buildRequest(Types... args, GET_APP_INFO_Tag)
    {
        return {};
    }

    template <typename... Types, REQUEST_TYPE T>
    web::http::http_request internal_buildRequest(Types... args, GET_CLUSTER_NS_INFO_Tag)
    {
        return {};
    }

    template <typename... Types, REQUEST_TYPE T>
    web::http::http_request buildRequest(Types &&... args)
    {
        return internal_buildRequest(std::forward<Types>(args)..., typename TagDispatchTrait<T>::Tag{});
    }

    /////////////////////////////////////////////////////////////////

    ////////////////////////////////         build base_url!         ////////////////////////////////

    /**
     * @brief  build a url for get a app's all namespace info.
     *         URL : http://{portal_address}/openapi/v1/envs/{env}/apps/{appId}/clusters/{clusterName}/namespaces
     *         Method: GET
     * @return
     */
    std::string buildGetAllNsUrl();

    /**
     * @brief build a url for PUT yaml config's modify.
     *        URL ： http://{portal_address}/openapi/v1/envs/{env}/apps/{appId}/clusters/{clusterName}/namespaces/{namespaceName}/items/{key}
     *        Method ： PUT
     *        for yaml or other no-properties config, key is content.
     * @param yamlFilename
     * @return
     */
    std::string buildModifyYamlConfigUrl(const std::string &yamlFilename);

    /**
     * @brief build a url for PUT properties config.
     * @param namespaceName
     * @param key
     * @return
     */
    std::string buildModifyPropertiesConfigUrl(const std::string &namespaceName, const std::string &key = "");

    /**
     * @brief build a url for POST to publish special namespace.
     *        URL ： http://{portal_address}/openapi/v1/envs/{env}/apps/{appId}/clusters/{clusterName}/namespaces/{namespaceName}/releases
     *        Method ： POST
     * @param publishNsName
     * @return
     */
    std::string buildPublishUrl(const std::string &publishNsName);

    /**
     * @brief build a url for DELETE properties key-value pair.
     *        URL ： http://{portal_address}/openapi/v1/envs/{env}/apps/{appId}/clusters/{clusterName}/namespaces/{namespaceName}/items/{key}?operator={operator}
     *        Method ： DELETE
     *        for no-properties config, this will delete all config!
     * @param namespaceName
     * @return
     */
    std::string buildDeleteUrl(const std::string &namespaceName);

    /**
     * @brief build a url for POST Add New config key-value pair.
     *        URL ： http://{portal_address}/openapi/v1/envs/{env}/apps/{appId}/clusters/{clusterName}/namespaces/{namespaceName}/items
     *        Method ： POST
     * @param namespaceName
     * @return
     */
    std::string buildAddConfigUrl(const std::string &namespaceName);

    ////////////////////////////////          build request         ////////////////////////////////

    /**
     * @brief build a request for get all app info.
     * @param apps
     * @return
     */
    web::http::http_request buildGetAppInfoRequest(const std::string &apps = "");

    /**
     * @brief build a request for get all namespace in a appId.
     * @return
     */
    web::http::http_request buildGetAllNsRequest();

    /**
     * @brief build a request for Modify Yaml config. Notice: this only use for yaml config.
     * @param node
     * @param comment
     * @param dataChangeLastModifiedBy
     * @param createIfNotExist
     * @param createdUserId
     * @return
     */
    web::http::http_request buildModifyYamlConfigRequest(const YAML::Node &node,
                                                         const std::string &comment = "",
                                                         const std::string &dataChangeLastModifiedBy = "apollo",
                                                         bool createIfNotExist = false,
                                                         const std::string &createdUserId = "apollo");

    /**
     * @brief build a request for modify properties config.
     * @param key
     * @param value
     * @param comment
     * @param dataChangeLastModifiedBy
     * @param createIfNotExist
     * @param createdUserId
     * @return
     */
    web::http::http_request buildModifyPropertiesConfigRequest(const std::string &key,
                                                               const std::string &value,
                                                               const std::string &comment = "",
                                                               const std::string &dataChangeLastModifiedBy = "apollo",
                                                               bool createIfNotExist = true,
                                                               const std::string &createdUserId = "apollo");

    /**
     * @brief build a request for publish one special namespace(configfile).
     * @param releaseTitle
     * @param releasedBy
     * @param releaseComment
     * @return
     */
    web::http::http_request buildPublishRequest(const std::string &releaseTitle,
                                                const std::string &releasedBy,
                                                const std::string &releaseComment);

    /**
     * @brief build a delete request for delete config pair
     * @param deleteUserId
     * @param key
     * @return
     */
    web::http::http_request buildDeleteRequest(const std::string &deleteUserId,
                                               const std::string &key = "content");

    /**
     * @brief build a add config request for add new config pair
     * @param key
     * @param value
     * @param comment
     * @param dataChangeCreatedBy
     * @return
     */
    web::http::http_request buildAddConfigRequest(const std::string &key,
                                                  const std::string &value,
                                                  const std::string &comment = "",
                                                  const std::string &dataChangeCreatedBy = "apollo");

    ////////////////////////////////////// exec Http request //////////////////////////////////////////
    web::http::http_response execHttpRequest(const std::string &base_url, const web::http::http_request &req);

    ///////////////////////////////////////  inner interface  /////////////////////////////////////////////////////

    /**
     * @brief update all namespace info for this appid
     */
    void updateAllNamespaceInfoInThisAppid();

    /**
     * @brief modify the yaml config, in fact, it change all file.
     * @param YamlFileName
     * @param node
     * @param comment
     * @param dataChangeLastModifiedBy
     * @param createIfNotExist
     * @param createdUserId
     * @return
     */
    bool modifyYamlConfig(const std::string &YamlFileName,
                          const YAML::Node &node,
                          const std::string &comment = "",
                          const std::string &dataChangeLastModifiedBy = "apollo",
                          bool createIfNotExist = false,
                          const std::string &createdUserId = "apollo");

    /**
     * @brief modify the properties config
     * @param namespaceName
     * @param key
     * @param value
     * @param comment
     * @param dataChangeLastModifiedBy
     * @param createIfNotExist
     * @param createdUserId
     * @return
     */
    bool modifyPropertiesConfig(const std::string &namespaceName,
                                const std::string &key,
                                const std::string &value,
                                const std::string &comment = "",
                                const std::string &dataChangeLastModifiedBy = "apollo",
                                bool createIfNotExist = false,
                                const std::string &createdUserId = "apollo");

    /**
     * @brief deal with all namespace config.
     * @param value
     */
    void dealWithJsonData(const web::json::value &value);

    /**
     * @brief delete a give key.(for properties, yaml use modify)
     * @param ns
     * @param deleteUserId
     * @param key
     * @return
     */
    bool deleteGiveItem(const std::string &ns,
                        const std::string &deleteUserId,
                        const std::string &key);

    /**
     * @brief add a new properties config key-value pair. (yaml use modify).
     * @param ns
     * @param key
     * @param value
     * @param comment
     * @param dataChangeCreatedBy
     * @return
     */
    bool addNewItem(const std::string &ns,
                    const std::string &key,
                    const std::string &value,
                    const std::string &comment,
                    const std::string &dataChangeCreatedBy);

    ///////////////////////////////////////////// core public api for user //////////////////////////////////////////////////

public:
    /**
     * @brief  get a config by a key. This key not include appid prefix and namespace prefix.
     *         Must give the namespace, because multi namespace may have same key.
     * @param appid
     * @param namespaceName
     * @param key
     * @return
     */
    std::optional<YAML::Node> getConfig(const std::string &appid,
                                        const std::string &namespaceName,
                                        const std::string &key);

    /**
     * @brief  change a exist config.
     * @param key
     * @param value
     * @param appid
     * @param ns
     * @return
     */
    bool setConfig(const std::string &key,
                   const std::string &value,
                   const std::string &namespaceName,
                   const std::string &appid = "");

    /**
     * @brief delete a exist config.
     * @param appid
     * @param ns
     * @param key
     * @param deleteUsrId
     * @return
     */
    bool deleteConfig(const std::string &appid, const std::string &ns,
                      const std::string &key,
                      const std::string &deleteUsrId = "apollo");

    /**
     * @brief add a new Config,
     *        must give appid,ns,key,value.
     * @param appid
     * @param ns
     * @param key
     * @param value
     * @param comment
     * @param dataChangeCreatedBy
     * @return
     */
    bool addNewConfig(const std::string &appid,
                      const std::string &ns,
                      const std::string &key,
                      const std::string &value,
                      const std::string &comment = "",
                      const std::string &dataChangeCreatedBy = "apollo");

    template <typename T>
    std::enable_if_t<(std::is_same_v<std::string, T> || apollo_client::is_NonChangeAble_v<T>), bool>
    addNewConfig(const std::string &appid,
                 const std::string &ns,
                 const std::string &key,
                 const T &value,
                 const std::string &comment = "",
                 const std::string &dataChangeCreatedBy = "apollo")
    {
        if ((g_map.count(ns) <= 0) && (g_node_map.count(ns)) <= 0) {
            SPDLOG_ERROR("namespace not exist! : {}", ns);
            return false;
        }
        if (apollo_client::ends_with(ns, std::string(".yaml")) || apollo_client::ends_with(ns, std::string(".xml"))) {
            // check exist.
            auto store_map = g_map[ns];
            std::string realKey = (appid + "/" + ns + "/" + key);
            if (store_map.count(realKey) > 0) { //exist just Modify.
                return setConfig(key, value, ns, appid);
            } else {
                YAML::Node value_node;
                auto rootNode = g_node_map[ns];
                if (rootNode.IsNull()) {
                    SPDLOG_INFO("This is an empty config! {}", ns);
                    value_node[key] = value;
                    std::stringstream ss;
                    ss << value_node;
                    if (!addNewItem(ns, "content", ss.str(), comment, dataChangeCreatedBy)) {
                        return false;
                    }
                    rootNode = value_node;
                } else {
                    //wrapper value to YAML::Node.
                    value_node = value;
                    //non-empty just modify it!
                    addNameYamlNode(rootNode, key, value_node);
                    if (!modifyYamlConfig(ns, rootNode, comment)) {
                        //TODO: recover new add node.
                        return false;
                    }
                }
                // add to store_map
                store_map[realKey] = value_node;
                return true;
            }
        } else { //properties
            if (ns.find('.') > ns.size()) {
                SPDLOG_ERROR("错误的ns!");
            } else {
                addNewItem(ns, key, value, comment, dataChangeCreatedBy);
            }
        }
        return true;
    }

    /**
     * @brief publish a namespace.
     * @param namespaceName
     * @param releaseTitle
     * @param releasedBy
     * @param releaseComment
     * @return
     */
    bool publishNamespace(const std::string &namespaceName,
                          const std::string &releaseTitle,
                          const std::string &releasedBy = "apollo",
                          const std::string &releaseComment = "comment");

    /**
     * @brief get all keys in this app
     * @return
     */
    std::optional<std::vector<std::string>> getAllKey();

    /**
     * @brief get all keys belongs to a namespace.
     * @param namespaceName
     * @return
     */
    std::optional<std::vector<std::string>> getNamespaceAllKey(const std::string &namespaceName);

    /**
     * @brief get all namespaces in this app
     * @return
     */
    std::optional<std::vector<std::string>> getAllNamespace();

    bool keyExist(const std::string &appid,
                  const std::string &namespaceName,
                  const std::string &key);

private:
    /**
     * @brief generate a header by token.
     * @param token
     * @return
     */
    static web::http::http_headers getTokenHeader(const std::string &token)
    {
        web::http::http_headers headers;
        headers.add(U("Authorization"), U(token.c_str()));
        headers.add(U("Content-Type"), U("application/json;charset=UTF-8"));
        return headers;
    }

    static std::string getNsNameByKey(const std::string &key)
    {
        size_t first = key.find('/');
        size_t second = key.find('/', first + 1);
        size_t third = key.find('/', second + 1);
        std::string result = key.substr(second + 1, third - second - 1);
        return result;
    }

    static std::string getTailNameByKey(const std::string &key)
    {
        size_t pos = key.find_last_of('/');
        std::string tailName = key.substr(pos + 1);
        return tailName;
    }

    static std::string getRealKey(const std::string &appid,
                                  const std::string &namespaceName, const std::string &key)
    {
        return (appid + "/" + namespaceName + "/" + key);
    }

public:
private:
    // <namespace : map<appid/namespace/namespaceInnerkey, YAML::Node>>
    std::map<std::string, std::map<std::string, YAML::Node>> g_map;
    // < namespace, namespaceALLInfo Node>
    std::map<std::string, YAML::Node> g_node_map;
    std::string token_{};
    std::vector<std::string> namespaces_;
    apollo_client::MultiNsConfig config_;
}; // namespace apollo_client

} // namespace apollo_client

#endif //APOLLO_CPP_CLIENT_APOLLO_OPENAPI_BASE_H
