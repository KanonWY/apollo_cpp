#include "apollo_base.h"
#include <spdlog/spdlog.h>
#include "sample_base.h"

void test_get_config_nobuffer_yaml()
{
    test_env env = {"http://localhost:8080", "SampleApp", "test_yaml", "default"};
    auto res = apollo_client::apollo_base::getConfigNoBufferInnerByYAML(env.base_url_, env.appid_, env.namespace_, env.cluster_);
    for (const auto &item : res) {
    }
}

void test_get_config_nobuffer()
{
    test_env env = {"http://localhost:8080", "SampleApp", "application", "default"};
    auto res = apollo_client::apollo_base::getConfigNoBufferInner(env.base_url_, env.appid_, env.namespace_, env.cluster_);
    for (const auto &item : res) {
        SPDLOG_INFO("key = {}, value = {}", item.first.c_str(), item.second.c_str());
    }
}

void test_yaml()
{
    test_env env = {"http://kanon2020.top:8080", "SampleApp", "test_yaml.yaml", "default"};
    auto res = apollo_client::apollo_base::getConfig(env.base_url_, env.appid_, env.namespace_, env.cluster_);
    if (res.has_value()) {
        SPDLOG_INFO(" config \n {}", res.value());
    }
}

void test_xml()
{
    test_env env = {"http://kanon2020.top:8080", "SampleApp", "test_xml.xml", "default"};
    auto res = apollo_client::apollo_base::getConfig<apollo_client::XmlPolicy>(env.base_url_, env.appid_, env.namespace_, env.cluster_);
    if (res.has_value()) {
        SPDLOG_INFO(" config \n {}", res.value());
    }
}

void test_properties()
{
    test_env env = {"http://kanon2020.top:8080", "SampleApp", "application", "default"};
    auto res = apollo_client::apollo_base::getConfig<apollo_client::PropertiesPolicy>(env.base_url_, env.appid_, env.namespace_, env.cluster_);
    if (res.has_value()) {
        SPDLOG_INFO("config is \n {}", res.value());
    }
}

int main()
{
    test_properties();
    return 0;
}
