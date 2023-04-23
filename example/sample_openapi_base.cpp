#include <iostream>
#include "../include/apollo_openapi_base.h"

static std::string token = "eafb8c5d6c45c8c11961ee60a74e8f04775596b08efaf4869807060e8af1eca5";
static std::string Address = "http://localhost:8070";

std::string strNode(YAML::Node &node)
{
    std::ostringstream ss;
    ss << node;
    return ss.str();
}

void dumpNode(YAML::Node &node)
{
    if (node.IsScalar()) {
        SPDLOG_INFO("value is{}", node.as<std::string>());
    } else if (node.IsSequence()) {
        auto res = node.as<std::vector<double>>();
        SPDLOG_INFO("{}", strNode(node));
    }
}

void TestGetAllAppNs()
{
    apollo_client::apollo_ctrl_base base;
    apollo_client::MultiNsConfig config;
    config.SetAppid("openapp").SetAddress(Address);
    base.init(token, config);

    // for (const auto &it : res) {
    //     std::cout << it << std::endl;
    // }
    auto res = base.getAllKey();
    for (const auto &item : res.value()) {
        std::cout << item << std::endl;
    }
}

void TestGetConfig_for_vec_double()
{
    apollo_client::apollo_ctrl_base base;
    apollo_client::MultiNsConfig config;
    config.SetAppid("openapp").SetAddress(Address);
    base.init(token, config);
    std::string key = "properties/cloud_range/value";
    auto res = base.getConfig("openapp", "testyaml.yaml", key);
    if (res.has_value()) {
        SPDLOG_INFO("key [{}] get success!", key);
        dumpNode(res.value());
    } else {
        SPDLOG_INFO("key [{}] get error!", key);
    }
}
void TestGetConfig_for_vec_string()
{
    apollo_client::apollo_ctrl_base base;
    apollo_client::MultiNsConfig config;
    config.SetAppid("openapp").SetAddress(Address);
    base.init(token, config);
    std::string key = "properties/kenon";
    auto res = base.getConfig("openapp", "testyaml.yaml", key);
    if (res.has_value()) {
        SPDLOG_INFO("key [{}] get success!", key);
        auto r = res.value().as<std::vector<std::string>>();
        for (const auto &it : r) {
            SPDLOG_INFO("{}", it);
        }
    } else {
        SPDLOG_INFO("key [{}] get error!", key);
    }
}

// 对于一个新创建的yaml类型的配置，是无法使用修改接口为其进行修改的，这时候需要调用addConfig的请求。
void TestAddConfig()
{
    apollo_client::apollo_ctrl_base base;
    apollo_client::MultiNsConfig config;
    config.SetAppid("openapp").SetAddress(Address);
    base.init(token, config);
    std::string key = "jhui";
    std::string value = "ldplpd";
    auto res = base.addNewConfig("openapp", "test_yaml2.yaml", key, value);
    if (res) {
        SPDLOG_INFO("add config success!");
    } else {
        SPDLOG_ERROR("add config error: {}", key);
    }
}

void TestPublish()
{
    apollo_client::apollo_ctrl_base base;
    apollo_client::MultiNsConfig config;
    config.SetAppid("openapp").SetAddress(Address);
    base.init(token, config);
    std::string ns = "test_yaml2.yaml";
    std::string pubTitle = "发布";
    auto res = base.publishNamespace(ns, pubTitle);
    if (res) {
        SPDLOG_INFO("publish {} success ", ns);
    } else {
        SPDLOG_INFO("publish {} error ", ns);
    }
}

void TestGetNamespaceKey()
{
    apollo_client::apollo_ctrl_base base;
    apollo_client::MultiNsConfig config;
    config.SetAppid("openapp").SetAddress(Address);
    base.init(token, config);
    auto r = base.getNamespaceAllKey("test_yaml2.yaml");
    if (r.has_value()) {
        SPDLOG_INFO("exist");
        for (const auto &item : r.value()) {
            SPDLOG_INFO("{}", item);
        }
    }
}

void TestGetAllKey()
{
    apollo_client::apollo_ctrl_base base;
    apollo_client::MultiNsConfig config;
    config.SetAppid("openapp").SetAddress(Address);
    base.init(token, config);
    auto r = base.getAllKey();
    if (r.has_value()) {
        SPDLOG_INFO("exist");
        for (const auto &item : r.value()) {
            SPDLOG_INFO("{}", item);
        }
    }
}

void TestGetAllNamespace()
{
    apollo_client::apollo_ctrl_base base;
    apollo_client::MultiNsConfig config;
    config.SetAppid("openapp").SetAddress(Address);
    base.init(token, config);
    auto r = base.getAllNamespace();
    if (r.has_value()) {
        SPDLOG_INFO("存在--->");
        for (const auto &item : r.value()) {
            SPDLOG_INFO("{}", item);
        }
    }
}

void TestDeleteConfig()
{
    apollo_client::apollo_ctrl_base base;
    apollo_client::MultiNsConfig config;
    config.SetAppid("openapp").SetAddress(Address);
    base.init(token, config);
    std::string key = "hello";
    auto res = base.deleteConfig("openapp", "testyaml.yaml", key);
    if (res) {
        SPDLOG_INFO("delete {} success!", key);
    } else {
        SPDLOG_INFO("delete {} error!", key);
    }
}

int main()
{
    TestDeleteConfig();
    return 0;
}