#include <iostream>
#include "apollo_openapi_base.h"

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
    for (const auto &item : res) {
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

void addConfig()
{
    apollo_client::apollo_ctrl_base base;
    apollo_client::MultiNsConfig config;
    config.SetAppid("openapp").SetAddress(Address);
    base.init(token, config);
    auto res = base.addNewConfig("openapp", "testyaml.yaml", "properties/kdokaokdo", "------------------->>>>");
    if (res) {
        SPDLOG_INFO("addNewConfig Success!");
        int a;
        SPDLOG_INFO("====> test success");
        std::cin >> a;
        auto r = base.getConfig("openapp", "testyaml2.yaml", "properties/Hello");
        if (r.has_value()) {
            SPDLOG_INFO("hello have this key!");
            if (r.value().IsScalar()) {
                SPDLOG_INFO("value is {}", r.value().as<std::string>());
            }
        }
    }
}

int main()
{
    TestAddConfig();
    return 0;
}