#include "apollo_openapi_base.h"

namespace apollo_client
{
std::string apollo_ctrl_base::buildGetAllNsUrl()
{
    std::ostringstream ss;
    ss << config_.address_ << "/openapi/v1/envs/" << config_.env_ << "/apps/" << config_.appid_
       << "/clusters/" << config_.cluster_ << "/namespaces";
    SPDLOG_INFO("GetAllNsUrl = {}", ss.str());
    return ss.str();
}

std::string apollo_ctrl_base::buildModifyYamlConfigUrl(const std::string &yamlFilename)
{
    std::ostringstream ss;
    ss << config_.address_ << "/openapi/v1/envs/" << config_.env_ << "/apps/" << config_.appid_ << "/clusters/"
       << config_.cluster_ << "/namespaces/" << yamlFilename << "/items/content";
    SPDLOG_INFO("buildModifyYamlConfigUrl = {}", ss.str());
    return ss.str();
}

std::string apollo_ctrl_base::buildModifyPropertiesConfigUrl(const std::string &namespaceName,
                                                             const std::string &key)
{
    std::ostringstream ss;
    ss << config_.address_ << "/openapi/v1/envs/" << config_.env_ << "/apps/" << config_.appid_ << "/clusters/"
       << config_.cluster_ << "/namespaces/" << namespaceName << "/items/";
    SPDLOG_INFO("buildModifyYamlConfigUrl = {}", ss.str());
    return ss.str();
}

std::string apollo_ctrl_base::buildPublishUrl(const std::string &publishNsName)
{
    std::ostringstream ss;
    ss << config_.address_ << "/openapi/v1/envs/" << config_.env_ << "/apps/" << config_.appid_ << "/clusters/"
       << config_.cluster_ << "/namespaces/" << publishNsName << "/releases";
    SPDLOG_INFO("PublishUrl = {}", ss.str());
    return ss.str();
}

std::string apollo_ctrl_base::buildDeleteUrl(const std::string &namespaceName)
{
    std::ostringstream ss;
    ss << config_.address_ << "/openapi/v1/envs/" << config_.env_ << "/apps/" << config_.appid_ << "/clusters/"
       << config_.cluster_ << "/namespaces/" << namespaceName << "/items";
    SPDLOG_INFO("PublishUrl = {}", ss.str());
    return ss.str();
}

std::string apollo_ctrl_base::buildAddConfigUrl(const std::string &namespaceName)
{
    std::ostringstream ss;
    ss << config_.address_ << "/openapi/v1/envs/" << config_.env_ << "/apps/" << config_.appid_ << "/clusters/"
       << config_.cluster_ << "/namespaces/" << namespaceName << "/items";
    SPDLOG_INFO("buildAddConfigUrl = {}", ss.str());
    return ss.str();
}

web::http::http_request apollo_ctrl_base::buildGetAppInfoRequest(const std::string &apps)
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

web::http::http_request apollo_ctrl_base::buildGetAllNsRequest()
{
    web::http::http_request request(web::http::methods::GET);
    auto headers = getTokenHeader(token_);
    request.headers() = headers;
    return request;
}

web::http::http_request apollo_ctrl_base::buildModifyYamlConfigRequest(const YAML::Node &node,
                                                                       const std::string &comment,
                                                                       const std::string &dataChangeLastModifiedBy,
                                                                       bool createIfNotExist,
                                                                       const std::string &createdUserId)
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
    //    SPDLOG_INFO("buildModifyYamlConfigRequest {}", ss.str());
    data[U("value")] = web::json::value::string(ss.str());
    data[U("comment")] = web::json::value::string(comment);
    data[U("dataChangeLastModifiedBy")] = web::json::value::string(dataChangeLastModifiedBy);
    if (createIfNotExist) {
        web::http::uri_builder builder;
        builder.append_query(U("createIfNotExists"), true);
        request.set_request_uri(builder.to_string());
        data[U("dataChangeCreatedBy")] = web::json::value::string(createdUserId);
    }
    request.set_body(data);
    return request;
}

web::http::http_request apollo_ctrl_base::buildModifyPropertiesConfigRequest(const std::string &key,
                                                                             const std::string &value,
                                                                             const std::string &comment,
                                                                             const std::string &dataChangeLastModifiedBy,
                                                                             bool createIfNotExist,
                                                                             const std::string &createdUserId)
{
    web::http::http_request request(web::http::methods::PUT);
    auto headers = getTokenHeader(token_);
    request.headers() = headers;

    //body for get!
    web::json::value data;
    data[U("key")] = web::json::value::string(key);
    data[U("value")] = web::json::value::string(value);
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

web::http::http_request apollo_ctrl_base::buildPublishRequest(const std::string &releaseTitle,
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

web::http::http_request apollo_ctrl_base::buildDeleteRequest(const std::string &deleteUserId,
                                                             const std::string &key)
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

web::http::http_request apollo_ctrl_base::buildAddConfigRequest(const std::string &key,
                                                                const std::string &value,
                                                                const std::string &comment,
                                                                const std::string &dataChangeCreatedBy)
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

web::http::http_response apollo_ctrl_base::execHttpRequest(const std::string &base_url, const web::http::http_request &req)
{
    try {
        auto requestClient = web::http::client::http_client(base_url);
        return requestClient.request(req).get();
    } catch (std::exception &e) {
        SPDLOG_ERROR("Exception: {}", e.what());
        return {};
    }
}

void apollo_ctrl_base::updateAllNamespaceInfoInThisAppid()
{
    auto response = execHttpRequest(buildGetAllNsUrl(),
                                    buildGetAllNsRequest());
    if (response.status_code() != web::http::status_codes::OK) {
        SPDLOG_ERROR("updateAllNamespace Error");
    } else {
        auto jsondata_from_server = response.extract_json().get();
        dealWithJsonData(jsondata_from_server);
    }
}

bool apollo_ctrl_base::modifyYamlConfig(const std::string &YamlFileName,
                                        const YAML::Node &node,
                                        const std::string &comment,
                                        const std::string &dataChangeLastModifiedBy,
                                        bool createIfNotExist,
                                        const std::string &createdUserId)
{
    auto response = execHttpRequest(buildModifyYamlConfigUrl(YamlFileName),
                                    buildModifyYamlConfigRequest(node,
                                                                 comment,
                                                                 dataChangeLastModifiedBy,
                                                                 createIfNotExist,
                                                                 createdUserId));
    if (response.status_code() != web::http::status_codes::OK) {
        SPDLOG_ERROR("modifyYamlConfig error: {}", YamlFileName);
        return false;
    }
    auto jsonData = response.extract_json().get();
    return true;
}

bool apollo_ctrl_base::modifyPropertiesConfig(const std::string &namespaceName,
                                              const std::string &key,
                                              const std::string &value,
                                              const std::string &comment,
                                              const std::string &dataChangeLastModifiedBy,
                                              bool createIfNotExist,
                                              const std::string &createdUserId)
{
    auto response = execHttpRequest(buildModifyPropertiesConfigUrl(namespaceName),
                                    buildModifyPropertiesConfigRequest(key,
                                                                       value,
                                                                       comment,
                                                                       dataChangeLastModifiedBy,
                                                                       createIfNotExist,
                                                                       createdUserId));
    if (response.status_code() != web::http::status_codes::OK) {
        SPDLOG_ERROR("modifyPropertiesConfig error: {}", key);
        return false;
    }
    return true;
}

void apollo_ctrl_base::dealWithJsonData(const web::json::value &value)
{
    YAML::Node node = YAML::Load(value.serialize().c_str());
    // node is a array type node.
    for (int i = 0; i < node.size(); ++i) {
        // change node to NsStore
        auto ns_store = node[i].as<NsStore>();
        namespaces_.push_back(ns_store.namespaceName.c_str());
        // this namespace file is properties
        if (!strcmp(ns_store.format.c_str(), "properties")) {
            // YAML::Node properties_node;
            for (auto &item : ns_store.items) {
                // properties need not to use g_node_map.
                // get echo key value in items
                // item["key"] -> string
                // item["value"] -> string
                // for easy
                YAML::Node value_node{item["value"]};
                // properties_node[item["key"]] = item["value"];
                std::string realKey = (config_.appid_ + "/" + ns_store.namespaceName + "/" + item["key"]);
                // insert store
                g_map[ns_store.namespaceName][realKey] = value_node;
            }
            //change tree
            // g_node_map[ns_store.namespaceName] = properties_node;
        } else {
            //namespace is non-properties
            // key always is "content".
            // value is a string.
            YAML::Node nd;
            if (!ns_store.items.empty()) {
                auto content = ns_store.items[0]["value"];
                nd = YAML::Load(content);
                g_node_map[ns_store.namespaceName] = nd;
                // std::string prefix = ("/" + config_.appid_ + "/" + ns_store.namespaceName);
                std::string prefix = (config_.appid_ + "/" + ns_store.namespaceName);
                parseYamlNode(nd, g_map[ns_store.namespaceName], prefix);
            } else {
                // may be this non-properties have no key-value
                SPDLOG_ERROR("{} is empty!", ns_store.namespaceName.c_str());
                g_node_map[ns_store.namespaceName.c_str()] = nd;
            }
        }
    }
}

bool apollo_ctrl_base::publishNamespace(const std::string &namespaceName,
                                        const std::string &releaseTitle,
                                        const std::string &releasedBy,
                                        const std::string &releaseComment)
{
    auto response = execHttpRequest(buildPublishUrl(namespaceName),
                                    buildPublishRequest(releaseTitle, releasedBy, releaseComment));
    if (response.status_code() != web::http::status_codes::OK) {
        SPDLOG_ERROR("publishNamespace error: {}", namespaceName);
        return false;
    }
    return true;
}

bool apollo_ctrl_base::deleteGiveItem(const std::string &ns,
                                      const std::string &deleteUserId,
                                      const std::string &key)
{
    auto response = execHttpRequest(buildDeleteUrl(ns),
                                    buildDeleteRequest(deleteUserId, key));
    if (response.status_code() != web::http::status_codes::OK) {
        SPDLOG_ERROR("delete give item error: {}", key);
        return false;
    }
    return true;
}

bool apollo_ctrl_base::addNewItem(const std::string &ns,
                                  const std::string &key,
                                  const std::string &value,
                                  const std::string &comment,
                                  const std::string &dataChangeCreatedBy)
{
    auto response = execHttpRequest(buildAddConfigUrl(ns),
                                    buildAddConfigRequest(key, value, comment, dataChangeCreatedBy));
    if (response.status_code() != web::http::status_codes::OK) {
        SPDLOG_ERROR("add new item error: {} \n {}", key, response.extract_json().get().serialize().c_str());
        return false;
    }
    return true;
}

std::optional<YAML::Node> apollo_ctrl_base::getConfig(const std::string &appid,
                                                      const std::string &namespaceName,
                                                      const std::string &key)
{
    if (g_map.count(namespaceName) <= 0) {
        SPDLOG_ERROR("get config ns not exist!{}", namespaceName);
        return {};
    }
    auto store_map = g_map[namespaceName];
    std::string realKey = (appid + "/" + namespaceName + "/" + key);
    if (store_map.count(realKey) > 0) {
        return store_map[realKey];
    } else {
        return {};
    }
}

bool apollo_ctrl_base::setConfig(const std::string &key,
                                 const std::string &value,
                                 const std::string &namespaceName,
                                 const std::string &appid)
{
    if (g_map.count(namespaceName) <= 0) {
        return false;
    }
    std::string realKey = (appid + "/" + namespaceName + "/" + key);
    auto store_map = g_map[namespaceName];
    if (store_map.count(realKey) <= 0) {
        //no exist
        return false;
    } else {
        if (namespaceName.find('.') == std::string::npos) {
            YAML::Node value_node(value);
            store_map[realKey] = value_node;
            return true;
        } else {
            if (!apollo_client::ends_with(namespaceName, std::string(".yaml"))) {
                SPDLOG_ERROR("other .xxx config not supper!");
                return false;
            } else {
                store_map[realKey] = value;
                if (g_node_map.count(namespaceName) > 0) {
                    auto rootNode = g_node_map[namespaceName];
                    YAML::Node tmpFather;
                    modifyGiveNameYamlNode(rootNode, tmpFather, key, store_map[realKey]);
                    if (!modifyYamlConfig(namespaceName, rootNode)) {
                        // TODO: recover set node.
                        return false;
                    }
                    return true;
                } else {
                    SPDLOG_ERROR("setConfig error g_node_map no exist! {} ", namespaceName);
                    return false;
                }
            }
        }
    }
}

bool apollo_ctrl_base::deleteConfig(const std::string &appid, const std::string &ns, const std::string &key,
                                    const std::string &deleteUsrId)
{
    if (!keyExist(appid, ns, key)) {
        return false;
    }
    auto store_map = g_map[ns];
    std::string realKey = getRealKey(appid, ns, key);
    if (apollo_client::ends_with(ns, std::string(".yaml")) || apollo_client::ends_with(ns, std::string(".xml"))) {
        //get rootNode which store all YAML::Node in a yaml file!
        auto rootNode = g_node_map[ns];
        YAML::Node tmpFatherNode;
        deleteGiveNameYamlNode(rootNode, tmpFatherNode, key);
        if (!modifyYamlConfig(ns, rootNode)) {
            return false;
        }
    } else { //properties direct call http req is ok!
        // TODO check this is a properties
        if (ns.find('.') != std::string::npos) {
            SPDLOG_ERROR("no support delete type conifg{}", ns);
            return false;
        }
        if (!deleteGiveItem(ns, deleteUsrId, key)) {
            return false;
        }
    }
    store_map.erase(realKey);
    return true;
} // namespace apollo_client

bool apollo_ctrl_base::addNewConfig(const std::string &appid, const std::string &ns, const std::string &key,
                                    const std::string &value, const std::string &comment, const std::string &dataChangeCreatedBy)
{
    // check ns exist
    if ((g_map.count(ns) <= 0) && (g_node_map.count(ns) <= 0)) {
        SPDLOG_ERROR("namespace not exist: {}", ns);
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

std::optional<std::vector<std::string>> apollo_ctrl_base::getAllKey()
{
    std::vector<std::string> res;
    for (const auto &item : g_map) {
        for (const auto &it : item.second) {
            res.push_back(it.first);
        }
    }
    if (res.empty()) {
        return {};
    } else {
        return res;
    }
}

std::optional<std::vector<std::string>> apollo_ctrl_base::getNamespaceAllKey(const std::string &namespaceName)
{
    std::vector<std::string> res;
    auto size = g_map.count(namespaceName);
    if ((g_map.count(namespaceName) <= 0) || (g_map[namespaceName].empty())) {
        return {};
    }

    for (const auto &it : g_map[namespaceName]) {
        res.push_back(it.first);
    }
    return res;
}

std::optional<std::vector<std::string>> apollo_ctrl_base::getAllNamespace()
{
    if (namespaces_.empty()) {
        return {};
    } else {
        return namespaces_;
    }
}

bool apollo_ctrl_base::keyExist(const std::string &appid,
                                const std::string &namespaceName,
                                const std::string &key)
{
    if (g_map.count(namespaceName) <= 0) {
        SPDLOG_ERROR("namespace {} not exist!", namespaceName);
        return false;
    }
    std::string realKey = (appid + "/" + namespaceName + "/" + key);
    if (g_map[namespaceName].count(realKey) <= 0) {
        SPDLOG_ERROR("key {} not exist!", key);
        return false;
    } else {
        return true;
    }
}
}; // namespace apollo_client