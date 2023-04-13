#include <iostream>
#include "apollo_openapi_base.h"

int main1()
{
    apollo_client::apollo_ctrl_base base;
    //    eafb8c5d6c45c8c11961ee60a74e8f04775596b08efaf4869807060e8af1eca5
    //    base.init("eafb8c5d6c45c8c11961ee60a74e8f04775596b08efaf4869807060e8af1eca5");

    apollo_client::MultiNsConfig config;
    config.SetAppid("openapp").SetAddress("http://localhost:8070");
    base.init("eafb8c5d6c45c8c11961ee60a74e8f04775596b08efaf4869807060e8af1eca5", config);
    base.getAllAppNs();

    for (const auto &it : base.g_map) {
        for (const auto &i : it.second) {
            SPDLOG_ERROR("{}", i.first.c_str());
        }
    }
    return 0;
}

int main2()
{
    apollo_client::apollo_ctrl_base base;
    //    eafb8c5d6c45c8c11961ee60a74e8f04775596b08efaf4869807060e8af1eca5
    //    base.init("eafb8c5d6c45c8c11961ee60a74e8f04775596b08efaf4869807060e8af1eca5");

    apollo_client::MultiNsConfig config;
    config.SetAppid("openapp").SetAddress("http://localhost:8070");
    base.init("eafb8c5d6c45c8c11961ee60a74e8f04775596b08efaf4869807060e8af1eca5", config);
    base.getAllAppNs();
    auto res = base.getConfig("openapp", "testyaml.yaml", "properties/cloud_range/value");
    for (const auto &it : base.g_map) {
        for (const auto &i : it.second) {
            SPDLOG_ERROR("{}", i.first.c_str());
        }
    }
    if (res.has_value()) {
        if (res.value().IsScalar()) {
            SPDLOG_ERROR("====>>>>>>>>>>>>>>{}", res.value().as<std::string>());
        } else if (res.value().IsSequence()) {
            auto r = res.value().as<std::vector<double>>();
            for (const auto &item : r) {
                std::cout << "[[[[ i ]]] = " << item << std::endl;
            }
        }
    }
    return 0;
}

//当前默认key不包含ns前缀
int getConfig()
{
    apollo_client::apollo_ctrl_base base;
    //    eafb8c5d6c45c8c11961ee60a74e8f04775596b08efaf4869807060e8af1eca5
    //    base.init("eafb8c5d6c45c8c11961ee60a74e8f04775596b08efaf4869807060e8af1eca5");

    apollo_client::MultiNsConfig config;
    config.SetAppid("openapp").SetAddress("http://localhost:8070");
    base.init("eafb8c5d6c45c8c11961ee60a74e8f04775596b08efaf4869807060e8af1eca5", config);
    base.getAllAppNs();
    auto res = base.getConfig("openapp", "testyaml.yaml", "properties/in_lidar_topic/alias");
    if (res.has_value()) {
        if (res->IsScalar()) {
            SPDLOG_ERROR("{}", res.value().as<std::string>());
        }
    }
    return 0;
}
//测试设置和传送
int setConfig()
{
    apollo_client::apollo_ctrl_base base;
    apollo_client::MultiNsConfig config;
    config.SetAppid("openapp").SetAddress("http://localhost:8070");
    base.init("eafb8c5d6c45c8c11961ee60a74e8f04775596b08efaf4869807060e8af1eca5", config);
    base.getAllAppNs();
    auto res = base.setConfig("alias", "ytesuasdiuausi", "openapp", "testyaml.yaml");
    if (res) {
        int a;
        SPDLOG_INFO("开始publish_1 {} ", a);
        std::cin >> a;
        SPDLOG_INFO("开始publish_2 {} ", a);
        res = base.publishNamespace("testyaml.yaml", "ceshi1");
        if (res) {
            SPDLOG_INFO("上传成功!");
        }
        auto r = base.getConfig("openapp", "testyaml.yaml", "alias");
        if (r.has_value()) {
            SPDLOG_INFO("VALUE = {}", r.value().as<std::string>());
        } else {
            SPDLOG_INFO("VALUE NOT EXIST");
        }
    }
    return 0;
}

static std::string token = "eafb8c5d6c45c8c11961ee60a74e8f04775596b08efaf4869807060e8af1eca5";

void addConfig()
{
    apollo_client::apollo_ctrl_base base;
    apollo_client::MultiNsConfig config;
    config.SetAppid("openapp").SetAddress("http://localhost:8070");
    base.init(token, config);
    base.getAllAppNs();
    auto res = base.addNewConfig("openapp", "testyaml.yaml", "properties/kdokaokdo", "------------------->>>>");
    if (res) {
        SPDLOG_INFO("addNewConfig Success!");
        int a;
        SPDLOG_INFO("====> test success");
        std::cin >> a;
        auto r = base.getConfig("openapp", "testyaml.yaml", "properties/Hello");
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
    addConfig();
    return 0;
}