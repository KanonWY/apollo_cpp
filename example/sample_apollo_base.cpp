#include "apollo_base.h"
#include <spdlog/spdlog.h>

struct test_env
{
    std::string base_url_;
    std::string appid_;
    std::string namespace_;
    std::string cluster_;
};

void test_get_config_nobuffer_yaml()
{
    test_env env = {"http://localhost:8080", "SampleApp", "test_yaml", "default"};

    apollo_client::apollo_base base_client;
    auto res = base_client.getConfigNoBufferInnerByYAML(env.base_url_, env.appid_, env.namespace_, env.cluster_);
    for(const auto& item : res)
    {

    }
}

void test_get_config_nobuffer()
{
    test_env env = {"http://localhost:8080", "SampleApp", "application", "default"};
    apollo_client::apollo_base base_client;
    auto res = base_client.getConfigNoBufferInner(env.base_url_, env.appid_, env.namespace_, env.cluster_);
    for(const auto& item :res)
    {
        SPDLOG_INFO("key = {}, value = {}",item.first.c_str(), item.second.c_str());
    }
}

int main()
{
    test_get_config_nobuffer_yaml();
//    test_get_config_nobuffer();
    return 0;
}