#include "apollo_base.h"
#include <spdlog/spdlog.h>
#include "sample_base.h"



void test_getYamlConfig_1()
{
    test_env env = {"http://localhost:8080", "SampleApp", "test_yaml", "default"};
    apollo_client::apollo_base base;
    auto node = base.getYamlConfig<apollo_client::RE_TYPE::YAML_OBJECT>(env.base_url_, env.appid_, env.namespace_, env.cluster_);
    SPDLOG_INFO("{}",node.as<std::string>());

}

void test_getYamlConfig_2()
{
    test_env env = {"http://localhost:8080", "SampleApp", "test_yaml.yaml", "default"};
    apollo_client::apollo_base base;
    auto node = base.getYamlConfig<apollo_client::RE_TYPE::YAML_STRING>(env.base_url_, env.appid_, env.namespace_, env.cluster_);
    SPDLOG_INFO("content = \n{}",node.c_str());

}

void test_getConfigNoBufferInnerByYAML()
{
    test_env env = {"http://localhost:8080", "SampleApp", "test_yaml.yaml", "default"};
    apollo_client::apollo_base base;
    auto node = base.getConfigNoBufferInnerByYAML(env.base_url_, env.appid_, env.namespace_, env.cluster_);

}

int main()
{
    test_getYamlConfig_2();
    return 0;
}
