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
    }

public:
    //URL : http://{portal_address}/openapi/v1/apps
    //Method : GET
    void pullAllInfo()
    {
    }

    ////////////////Build base_url

    std::string buildBaseUrl() { return {}; }

    //URL : http://{portal_address}/openapi/v1/envs/{env}/apps/{appId}/clusters/{clusterName}/namespaces
    //Method: GET
    std::string buildGetAllNsUrl()
    {
        std::ostringstream ss;
        ss << config_.address_ << "/openapi/v1/envs/" << config_.env_ << "/apps/" << config_.appid_
           << "/clusters/" << config_.cluster_ << "/namespaces";
        SPDLOG_INFO("GetAllNsUrl = {}", ss.str());
        return ss.str();
    }

    //URL ： http://{portal_address}/openapi/v1/envs/{env}/apps/{appId}/clusters/{clusterName}/namespaces/{namespaceName}/items/{key}
    //Method ： PUT
    // for yaml: key = content
    std::string buildModifyYamlConfigUrl(const std::string &yamlFilename)
    {
        std::ostringstream ss;
        ss << config_.address_ << "/openapi/v1/envs/" << config_.env_ << "/apps/" << config_.appid_ << "/clusters/"
           << config_.cluster_ << "/namespaces/" << yamlFilename << "/items/content";
        SPDLOG_INFO("buildModifyYamlConfigUrl = {}", ss.str());
        return ss.str();
    }
    // URL ： http://{portal_address}/openapi/v1/envs/{env}/apps/{appId}/clusters/{clusterName}/namespaces/{namespaceName}/releases
    //Method ： POST

    std::string buildPublishUrl(const std::string &publishNsName)
    {
        std::ostringstream ss;
        ss << config_.address_ << "/openapi/v1/envs/" << config_.env_ << "/apps/" << config_.appid_ << "/clusters/"
           << config_.cluster_ << "/namespaces/" << publishNsName << "/releases";
        SPDLOG_INFO("PublishUrl = {}", ss.str());
        return ss.str();
    }

    //URL ： http://{portal_address}/openapi/v1/envs/{env}/apps/{appId}/clusters/{clusterName}/namespaces/{namespaceName}/items/{key}?operator={operator}
    //Method ： DELETE
    std::string buildDeleteUrl(const std::string &namespaceName)
    {
        std::ostringstream ss;
        ss << config_.address_ << "/openapi/v1/envs/" << config_.env_ << "/apps/" << config_.appid_ << "/clusters/"
           << config_.cluster_ << "/namespaces/" << namespaceName << "/items";
        SPDLOG_INFO("PublishUrl = {}", ss.str());
        return ss.str();
    }

    //URL ： http://{portal_address}/openapi/v1/envs/{env}/apps/{appId}/clusters/{clusterName}/namespaces/{namespaceName}/items
    //Method ： POST
    std::string buildAddConfigUrl(const std::string &namespaceName)
    {
        std::ostringstream ss;
        ss << config_.address_ << "/openapi/v1/envs/" << config_.env_ << "/apps/" << config_.appid_ << "/clusters/"
           << config_.cluster_ << "/namespaces/" << namespaceName << "/items";
        SPDLOG_INFO("PublishUrl = {}", ss.str());
        return ss.str();
    }

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

    //获取所有的 app info,包括自己没有管理的。
    web::http::http_request buildGetAppInfoRequest(const std::string &apps = "")
    {
        web::http::http_request request(web::http::methods::GET);
        auto headers = getTokenHeader(token_);
        request.headers() = headers;
        if (!apps.empty()) {
            //request param
            web::http::uri_builder builder;
            builder.append_query(U("appIds"), apps.c_str());
            request.set_request_uri(builder.to_string());
        }
        return request;
    }

    //获取appid下所有的namespace
    web::http::http_request buildGetAllNsRequest()
    {
        web::http::http_request request(web::http::methods::GET);
        auto headers = getTokenHeader(token_);
        request.headers() = headers;
        return request;
    }

    web::http::http_request buildModifyYamlConfigRequest(const YAML::Node &node,
                                                         const std::string &comment = "",
                                                         const std::string &dataChangeLastModifiedBy = "apollo",
                                                         bool createIfNotExist = false,
                                                         const std::string &createdUserId = "apollo")
    {
        // PUT method
        web::http::http_request request(web::http::methods::PUT);
        auto headers = getTokenHeader(token_);
        request.headers() = headers;

        //body for get!
        web::json::value data;
        data[U("key")] = web::json::value::string("content");
        std::stringstream ss;
        ss << node;
        SPDLOG_INFO("{}", ss.str());
        data[U("value")] = web::json::value::string(ss.str());
        data[U("comment")] = web::json::value::string(comment);
        data[U("dataChangeLastModifiedBy")] = web::json::value::string(dataChangeLastModifiedBy);
        if (createIfNotExist) {
            web::http::uri_builder builder;
            builder.append_query(U("createIfNotExists"), createIfNotExist);
            request.set_request_uri(builder.to_string());
            data[U("dataChangeCreatedBy")] = web::json::value::string(createdUserId);
        }
        request.set_body(data);
        return request;
    }

    web::http::http_request buildPushlishRequest(const std::string &releaseTitle,
                                                 const std::string &releasedBy,
                                                 const std::string &releaseComment)
    {
        web::http::http_request request(web::http::methods::POST);
        auto headers = getTokenHeader(token_);
        web::json::value data;
        data[U("releaseTitle")] = web::json::value::string(releaseTitle);
        data[U("releasedBy")] = web::json::value::string(releasedBy);
        data[U("releaseComment")] = web::json::value::string(releaseComment);
        request.headers() = headers;
        request.set_body(data);
        return request;
    }

    web::http::http_request buildDeleteRequest(const std::string &deleteUserId,
                                               const std::string &key = "content")
    {
        web::http::http_request request(web::http::methods::DEL);
        auto headers = getTokenHeader(token_);
        web::http::uri_builder builder;
        builder.append_query(U("key"), key);
        builder.append_query(U("operator"), deleteUserId);
        request.set_request_uri(builder.to_string());
        request.headers() = headers;
        return request;
    }

    web::http::http_request buildAddConfigRequest(const std::string &key,
                                                  const std::string &value,
                                                  const std::string &comment = "",
                                                  const std::string &dataChangeCreatedBy = "apollo")
    {
        web::http::http_request request(web::http::methods::POST);
        auto headers = getTokenHeader(token_);
        web::json::value data;
        data[U("key")] = web::json::value::string(key);
        data[U("value")] = web::json::value::string(value);
        data[U("comment")] = web::json::value::string(comment);
        data[U("dataChangeCreatedBy")] = web::json::value::string(dataChangeCreatedBy);
        request.headers() = headers;
        request.set_body(data);
        return request;
    }

    //////////////////////////////////////
    web::http::http_response execHttpRequest(const std::string &base_url, const web::http::http_request &req)
    {
        try {
            auto requestClient = web::http::client::http_client(base_url);
            return requestClient.request(req).get();
        } catch (std::exception &e) {
            SPDLOG_ERROR("Exception: {}", e.what());
            return {};
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void getAllAppNs()
    {
        auto res = execHttpRequest(buildGetAllNsUrl(), buildGetAllNsRequest());
        SPDLOG_INFO("status {}", res.status_code());
        auto jsonData = res.extract_json().get();
        dump(jsonData);
        dealWithJsonData(jsonData);
    }

    void dump(const web::json::value &value)
    {
        SPDLOG_INFO("value type = {} \n value size = {}", value.type(), value.size());
        SPDLOG_INFO("value content = {}", value.serialize().c_str());
    }

    bool modifyYamlConfig(const std::string &YamlFileName,
                          const YAML::Node &node,
                          const std::string &comment = "",
                          const std::string &dataChangeLastModifiedBy = "apollo",
                          bool createIfNotExist = false,
                          const std::string &createdUserId = "apollo")
    {
        auto res = execHttpRequest(buildModifyYamlConfigUrl(YamlFileName),
                                   buildModifyYamlConfigRequest(node,
                                                                comment,
                                                                dataChangeLastModifiedBy,
                                                                createIfNotExist,
                                                                createdUserId));
        SPDLOG_INFO("status modifyYamlConfig {}", res.status_code());
        auto jsonData = res.extract_json().get();
        dump(jsonData);
        return true;
    }

    void dealWithJsonData(const web::json::value &value)
    {
        YAML::Node node = YAML::Load(value.serialize().c_str());
        SPDLOG_INFO("node type = {}", node.Type());
        for (int i = 0; i < node.size(); ++i) {
            auto res = node[i].as<NsStore>();
            SPDLOG_INFO("ns = {}", res.namespaceName.c_str());
            namespaces_.push_back(res.namespaceName.c_str());
            if (!strcmp(res.format.c_str(), "properties")) {
                //config 为properties
                for (const auto &item : res.items) {
                    //       item["key"];
                    //       item["value"];
                }
            } else {
                //config 非 properties
                // key固定为content
                // value为一个字符串

                YAML::Node nd;
                if (!res.items.empty()) {
                    auto content = res.items[0]["value"];
                    nd = YAML::Load(content);
                    g_node_map[res.namespaceName.c_str()] = nd;
                    std::string prefix = ("/" + config_.appid_ + "/" + res.namespaceName);
                    parseYamlNode(nd, g_map[res.namespaceName], prefix);
                } else {
                    SPDLOG_ERROR("{} is empty!", res.namespaceName.c_str());
                    g_node_map[res.namespaceName.c_str()] = nd;
                }
            }
        }
    }

    bool publishNamespace(const std::string &ns,
                          const std::string &releaseTitle,
                          const std::string &releasedBy = "apollo",
                          const std::string &releaseComment = "comment")
    {
        auto res = execHttpRequest(buildPublishUrl(ns), buildPushlishRequest(releaseTitle, releasedBy, releaseComment));
        SPDLOG_INFO("publishNamespace status {} ", res.status_code());
        if (res.status_code() == web::http::status_codes::OK) {
            return true;
        } else {
            return false;
        }
    }

    bool deleteGiveItem(const std::string &ns,
                        const std::string &deleteUserId,
                        const std::string &key = "content")
    {
        auto res = execHttpRequest(buildDeleteUrl(ns),
                                   buildDeleteRequest(deleteUserId, key));
        SPDLOG_INFO("status deleteGiveItem {}", res.status_code());
        return true;
    }

    bool addNewItem(const std::string &ns,
                    const std::string &key,
                    const std::string &value,
                    const std::string &comment,
                    const std::string &dataChangeCreatedBy)
    {
        auto response = execHttpRequest(buildAddConfigUrl(ns),
                                        buildAddConfigRequest(key, value, comment, dataChangeCreatedBy));
        SPDLOG_INFO("status addNewConfig{}", response.status_code());
        return true;
    }

    /////////////////////////////////////////////

    static web::http::http_headers
    getTokenHeader(const std::string &token)
    {
        web::http::http_headers headers;
        headers.add(U("Authorization"), U(token.c_str()));
        headers.add(U("Content-Type"), U("application/json;charset=UTF-8"));
        return headers;
    }

    std::optional<YAML::Node> getConfig(const std::string &appid, const std::string &ns, const std::string &key)
    {
        auto store_map = g_map[ns];
        std::string realKey = ("/" + appid + "/" + ns + "/" + key);
        SPDLOG_INFO("realKey = {}", realKey);
        if (store_map.count(realKey) > 0) {
            return store_map[realKey];
        } else {
            return store_map[realKey];
        }
    }

    /**
     * @brief  key must exist!
     * @param key
     * @param value
     * @param appid
     * @param ns
     * @return
     */
    bool setConfig(const std::string &key, const std::string &value, const std::string &appid = "", const std::string &ns = "")
    {
        auto store_map = g_map[ns];
        std::string realKey = ('/' + appid + "/" + ns + "/" + key);
        if (store_map.count(realKey) > 0) {
            //exist -> modify!
            store_map[realKey] = value;
            auto belongNs = getNsNameByKey(realKey);
            auto tailName = getTailNameByKey(realKey);
            if (g_node_map.count(ns) > 0) {
                auto node = g_node_map[belongNs];
                YAML::Node tmpFa;
                modifyGiveNameYamlNode(node, tmpFa, key, store_map[realKey]);
                if (!modifyYamlConfig(ns, node)) {
                    //TODO: recover set node.
                    return false;
                }
                publishNamespace(ns, "release");
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
    }

    bool deleteConfig(const std::string &appid, const std::string &ns, const std::string &key,
                      const std::string &deleteUsrId = "apollo")
    {
        if (apollo_client::ends_with(ns, std::string(".yaml")) || apollo_client::ends_with(ns, std::string(".xml"))) {
            // get really store map.
            auto store_map = g_map[ns];
            std::string realKey = ("/" + appid + "/" + ns + "/" + key);
            if (store_map.count(realKey) > 0) {
                //get rootNode which store all YAML::Node in a yaml file!
                auto rootNode = g_node_map[ns];
                YAML::Node tmpFa;
                //remove node(key) from rootNode.
                deleteGiveNameYamlNode(rootNode, tmpFa, key);
                //call http modify req.
                if (!modifyYamlConfig(ns, rootNode)) {
                    //TODO: recover key-Node!
                    return false;
                }
                //erase node from map.
                store_map.erase(realKey);
                return true;
            } else {
                //no config, just return.
                return true;
            }

        } else { //properties direct call http req is ok!
            if (ns.find('.') == std::string::npos) {
                SPDLOG_ERROR("wrong namespace input: {}!", ns);
            } else {
                deleteGiveItem(ns, deleteUsrId, key);
            }
        }
        return true;
    }

    bool addNewConfig(const std::string &appid, const std::string &ns, const std::string &key,
                      const std::string &value, const std::string &comment = "", const std::string &dataChangeCreatedBy = "apollo")
    {
        if (apollo_client::ends_with(ns, std::string(".yaml")) || apollo_client::ends_with(ns, std::string(".xml"))) {
            // check exist.
            auto store_map = g_map[ns];
            std::string realKey = ("/" + appid + "/" + ns + "/" + key);
            if (store_map.count(realKey) > 0) { //exist just Modify.
                return setConfig(key, value, appid, ns);
            } else { // new will to add.
                //wrapper value to YAML::Node.
                YAML::Node value_node{value};
                //add to rootNode.
                auto rootNode = g_node_map[ns];
                addNameYamlNode(rootNode, key, value_node);
                //modify
                if (!modifyYamlConfig(ns, rootNode, comment)) {
                    //TODO: recover new add node.
                    return false;
                }
                // add to store_map
                store_map[realKey] = value_node;
                publishNamespace(ns, "release");
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

    //private:
    std::map<std::string, std::map<std::string, YAML::Node>> g_map;
    std::map<std::string, YAML::Node> g_node_map;
    std::string token_{};
    std::vector<std::string> namespaces_;
    apollo_client::MultiNsConfig config_;
};

} // namespace apollo_client

#endif //APOLLO_CPP_CLIENT_APOLLO_OPENAPI_BASE_H
