#include <cpprest/http_client.h>
#include "apollo_base.h"
#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>
#include "yaml_code_template.h"
#include "const_url_value.h"

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
            SPDLOG_INFO("jsonData content {}", jsonData.serialize().c_str());
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

bool apollo_base::getConfigNoBufferInner(const std::string &config_server_url,
                                         const std::string &appidName,
                                         const std::string &namespaceName,
                                         const std::string &clusterName,
                                         std::map<std::string, std::string> &output)
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

std::string apollo_base::getConfigNoBufferByKeyInner(const std::string &config_server_url,
                                                     const std::string &appidName,
                                                     const std::string &namespaceName,
                                                     const std::string &clusterName,
                                                     const std::string &key)
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
std::map<std::string, std::string> apollo_base::getConfigNoBufferInnerByYAML(const std::string &config_server_url,
                                                                             const std::string &appid_name,
                                                                             const std::string &namespace_name,
                                                                             const std::string &cluster_name)
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

            // try to use yaml_cpp to parse the content
            //get configurations from json object
            auto configJsonObj = jsonData[U(CONFIGURATIONS_NAME)].as_object();

            SPDLOG_INFO("configJsonObj = {}", jsonData[U(CONFIGURATIONS_NAME)].serialize().c_str());
            YAML::Node node = YAML::Load(jsonData[U(CONFIGURATIONS_NAME)].serialize().c_str());
            SPDLOG_INFO("node's type = {}", node.Type());
            if (node["content"]) {
                SPDLOG_INFO("conetnt = {}", node["content"].as<std::string>());
                YAML::Node nodeContent = YAML::Load(node["content"].as<std::string>());
                SPDLOG_INFO("nodeContent type = {}", nodeContent.Type());
                SPDLOG_INFO("nodeContent[version] = {}", nodeContent["version"].as<std::string>());
                SPDLOG_INFO("properties type = {}", nodeContent["properties"].Type());
                SPDLOG_INFO("properties[detected_objects_topic].type = {}", nodeContent["properties"]["bg_topic"]["value"].as<std::string>());
            }

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////// -----> apollo_openapi_base  <------ ////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void apollo_openapi_base::init(const std::string &token)
{
    token_ = token;
}

std::optional<std::string> apollo_openapi_base::getConfigNoProperties(const std::string &address,
                                                                      const std::string &env,
                                                                      const std::string &appid,
                                                                      const std::string &clustername,
                                                                      const std::string &namespacename)
{
    if ((address.size() + env.size() + clustername.size() + namespacename.size()) > 256) {
        SPDLOG_ERROR("to much url param");
    }
    std::map<std::string, std::string> resMap;
    std::string base_url;
    base_url.reserve(256);
    {
        char *url = new char[256];
        sprintf(url, "http://%s/openapi/v1/envs/%s/apps/%s/clusters/%s/namespaces/%s/items/content",
                address.c_str(), env.c_str(), appid.c_str(), clustername.c_str(), namespacename.c_str());
        base_url = url;
        delete[] url;
    }
    try {
        auto requestClient = web::http::client::http_client(base_url);
        // create header, add_token
        auto headers = getTokenHeader(token_);
        web::http::http_request request(web::http::methods::GET);
        request.headers() = headers;
        auto response = requestClient.request(request).get();

        if (response.status_code() != web::http::status_codes::OK) {
            SPDLOG_ERROR("Get {} error", base_url.c_str());
            return {};
        } else {
            auto json_data = response.extract_json().get();
            if (json_data.type() == web::json::value::value_type::Null) {
                SPDLOG_ERROR("Get {} error, return nullptr value!", base_url.c_str());
            } else if (json_data.type() == web::json::value::value_type::Object) {
                YAML::Node node = YAML::Load(json_data[U("value")].serialize().c_str());
                if (node.Type() == YAML::NodeType::value::Scalar) {
                    SPDLOG_INFO("node content = \n{}", node.as<std::string>());
                    return node.as<std::string>();
                }
            }
        }
    } catch (std::exception &e) {
        SPDLOG_ERROR("Exception: {}", e.what());
    }
    return {};
}

bool apollo_openapi_base::modifyConfigNoProperties(const std::string &address,
                                                   const std::string &env,
                                                   const std::string &appid,
                                                   const std::string &clustername,
                                                   const std::string &namespacename,
                                                   const std::string &conifgstr,
                                                   const std::string &modifyuserid,
                                                   bool createIfnotExists,
                                                   const std::string &comment,
                                                   const std::string &createuerid)
{
    if ((address.size() + env.size() + clustername.size() + namespacename.size()) > 256) {
        SPDLOG_ERROR("to much url param");
        return false;
    }
    std::string base_url;
    base_url.reserve(256);
    {
        char *url = new char[256];
        sprintf(url, "http://%s/openapi/v1/envs/%s/apps/%s/clusters/%s/namespaces/%s/items/content",
                address.c_str(), env.c_str(), appid.c_str(), clustername.c_str(), namespacename.c_str());
        base_url = url;
        delete[] url;
    }
    try {

        SPDLOG_INFO("URL = {}", base_url.c_str());
        auto requestClient = web::http::client::http_client(base_url);

        web::http::uri_builder builder;
        builder.append_query(U("createIfNotExists"), createIfnotExists);

        // create header, add_token
        auto headers = getTokenHeader(token_);
        // create body add changed file
        web::json::value data;
        data[U("key")] = web::json::value::string("content");
        data[U("value")] = web::json::value::string(conifgstr);
        data[U("dataChangeLastModifiedBy")] = web::json::value::string(modifyuserid);

        web::http::http_request request(web::http::methods::PUT);
        request.headers() = headers;
        request.set_request_uri(builder.to_string());
        request.set_body(data);
        auto response = requestClient.request(request).get();
        if (response.status_code() == web::http::status_codes::OK) {
            SPDLOG_INFO("modify success {}", response.status_code());
            return true;
        } else {
            SPDLOG_ERROR("modify error {}", response.status_code());
            return false;
        }
    } catch (std::exception &e) {
        SPDLOG_ERROR("Exception: {}", e.what());
        return false;
    }
}

bool apollo_openapi_base::deleteConfig(const std::string &address,
                                       const std::string &env,
                                       const std::string &appid,
                                       const std::string &clustername,
                                       const std::string &namespacename,
                                       const std::string &deleteuserid)
{
    if ((address.size() + env.size() + clustername.size() + namespacename.size()) > 256) {
        SPDLOG_ERROR("to much url param");
        return false;
    }
    std::string base_url;
    base_url.reserve(256);
    {
        char *url = new char[256];
        sprintf(url, "http://%s/openapi/v1/envs/%s/apps/%s/clusters/%s/namespaces/%s/items/content",
                address.c_str(), env.c_str(), appid.c_str(), clustername.c_str(), namespacename.c_str());
        base_url = url;
        delete[] url;
    }
    try {
        SPDLOG_INFO("URL = {}", base_url.c_str());
        auto requestClient = web::http::client::http_client(base_url);

        //TODO add request param.
        web::http::uri_builder builder;
        builder.append_query(U("key"), "content");
        builder.append_query(U("operator"), deleteuserid.c_str());

        // create header, add_token
        auto headers = getTokenHeader(token_);

        web::http::http_request request(web::http::methods::DEL);
        request.set_request_uri(builder.to_string());
        request.headers() = headers;

        auto response = requestClient.request(request).get();
        if (response.status_code() == web::http::status_codes::OK) {
            SPDLOG_INFO("delete config success {}", response.status_code());
            return true;
        } else {
            SPDLOG_ERROR("delete config error {}", response.status_code());
            return false;
        }
    } catch (std::exception &e) {
        SPDLOG_ERROR("Exception: {}", e.what());
        return false;
    }
}

bool apollo_openapi_base::publishConfig(const std::string &address,
                                        const std::string &env,
                                        const std::string &appid,
                                        const std::string &clusterName,
                                        const std::string &namespacename,
                                        const std::string &releaseTitle,
                                        const std::string &releasedBy,
                                        const std::string &releaseComment)
{
    if ((address.size() + env.size() + clusterName.size() + namespacename.size()) > 256) {
        SPDLOG_ERROR("to much url param");
        return false;
    }
    std::string base_url;
    base_url.reserve(256);
    {
        char *url = new char[256];
        sprintf(url, "http://%s/openapi/v1/envs/%s/apps/%s/clusters/%s/namespaces/%s/releases",
                address.c_str(), env.c_str(), appid.c_str(), clusterName.c_str(), namespacename.c_str());
        base_url = url;
        delete[] url;
    }
    try {
        SPDLOG_INFO("URL = {}", base_url.c_str());
        auto requestClient = web::http::client::http_client(base_url);

        //TODO add request param.

        // create header, add_token
        auto headers = getTokenHeader(token_);

        // create body add changed file
        web::json::value data;
        data[U("releaseTitle")] = web::json::value::string(releaseTitle);
        data[U("releasedBy")] = web::json::value::string(releasedBy);
        data[U("releaseComment")] = web::json::value::string(releaseComment);

        web::http::http_request request(web::http::methods::POST);
        request.headers() = headers;
        request.set_body(data);
        auto response = requestClient.request(request).get();
        if (response.status_code() == web::http::status_codes::OK) {
            SPDLOG_INFO("publishConfig success {}", response.status_code());
            return true;
        } else {
            SPDLOG_ERROR("publish config error {}", response.status_code());
            return false;
        }
    } catch (std::exception &e) {
        SPDLOG_ERROR("Exception: {}", e.what());
        return false;
    }
}
web::http::http_headers apollo_openapi_base::getTokenHeader(const std::string &token)
{
    web::http::http_headers headers;
    headers.add(U("Authorization"), U(token.c_str()));
    headers.add(U("Content-Type"), U("application/json;charset=UTF-8"));
    return headers;
}

std::string apollo_openapi_base::getAppenvInfo(const std::string &address, const std::string &appid)
{
    return {};
}

std::string apollo_openapi_base::getAppInfo(const std::string &address)
{
    return {};
}

std::string apollo_openapi_base::createCluster(const std::string &address, const std::string &envinfo, const std::string &appid)
{
    return {};
}

std::string apollo_openapi_base::getClusterInfo(const std::string &address, const std::string &envinfo, const std::string &appid, const std::string &clustername)
{
    return {};
}

std::string apollo_openapi_base::getNamespaceinfoInCluster(const std::string &address, const std::string &env, const std::string &appid, const std::string &clustername)
{
    return {};
}

std::string apollo_openapi_base::getSpecialNamespaceInfo(const std::string &address, const std::string &env, const std::string &appid, const std::string &clustername, const std::string &namespacesname)
{
    return {};
}

bool apollo_openapi_base::createNewNamespace(const std::string &address,
                                             const std::string &appid,
                                             const std::string &namespacename,
                                             const std::string &format,
                                             const std::string &dataChangeCreatedBy,
                                             bool isPublic,
                                             const std::string &comment)
{
    std::string base_url;
    base_url.reserve(256);
    {
        char *url = new char[256];
        sprintf(url, "http://%s/openapi/v1/apps/%s/appnamespaces",
                address.c_str(), appid.c_str());
        base_url = url;
        delete[] url;
    }
    try {
        auto requestClient = web::http::client::http_client(base_url);
        web::http::http_request request(web::http::methods::POST);
        auto headers = getTokenHeader(token_);
        web::json::value data;
        data[U("name")] = web::json::value::string(namespacename);
        data[U("appId")] = web::json::value::string(appid);
        data[U("format")] = web::json::value::string(format);
        data[U("isPublic")] = web::json::value::boolean(isPublic);
        data[U("comment")] = web::json::value::string(comment);
        data[U("dataChangeCreatedBy")] = web::json::value::string(dataChangeCreatedBy);
        request.headers() = headers;
        request.set_body(data);
        auto response = requestClient.request(request).get();
        SPDLOG_INFO("content = {}", response.extract_json().get().serialize().c_str());
        if (response.status_code() == web::http::status_codes::OK) {
            SPDLOG_INFO("create ns {}.{} success {}", namespacename, format, response.status_code());
            return true;
        } else {
            SPDLOG_ERROR("create ns error {}", response.status_code());
            return false;
        }
    } catch (std::exception &e) {
        SPDLOG_ERROR("Exception: {}", e.what());
    }
    return false;
}

bool apollo_openapi_base::createNewConfig(const std::string &address,
                                          const std::string &env,
                                          const std::string &appid,
                                          const std::string &clustername,
                                          const std::string &namespacename,
                                          const std::string &key,
                                          const std::string &value,
                                          const std::string &dataChangeCreatedBy,
                                          const std::string &comment)
{
    std::string base_url;
    base_url.reserve(256);
    {
        char *url = new char[256];
        sprintf(url, "http://%s/openapi/v1/envs/%s/apps/%s/clusters/%s/namespaces/%s/items",
                address.c_str(), env.c_str(), appid.c_str(), clustername.c_str(), namespacename.c_str());
        base_url = url;
        delete[] url;
    }
    try {
        SPDLOG_INFO("URL = {}", base_url);
        auto requestClient = web::http::client::http_client(base_url);
        web::http::http_request request(web::http::methods::POST);
        // build request.
        auto headers = getTokenHeader(token_);
        web::json::value data;
        data[U("key")] = web::json::value::string(key);
        data[U("value")] = web::json::value::string(value);
        data[U("comment")] = web::json::value::string(comment);
        data[U("dataChangeCreatedBy")] = web::json::value::string(dataChangeCreatedBy);
        request.headers() = headers;
        request.set_body(data);
        auto response = requestClient.request(request).get();
        SPDLOG_INFO("content = {}", response.extract_json().get().serialize().c_str());
        if (response.status_code() == web::http::status_codes::OK) {
            SPDLOG_INFO("ceate config {}, success {}", namespacename, response.status_code());
            return true;
        } else {
            SPDLOG_ERROR("create new config: {} error {}", namespacename, response.status_code());
            return false;
        }

    } catch (std::exception &e) {
        SPDLOG_ERROR("Exception: {}", e.what());
    }
    return false;
}

void apollo_openapi_base::rollbackConfig(const std::string &address, const std::string &env, const std::string &releaseid)
{
}

} // namespace apollo_client