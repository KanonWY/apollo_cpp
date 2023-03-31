#include "apollo_base.h"
#include "sample_base.h"

void test_get_conifg_by_openapi()
{
    test_env env = {"localhost:8070", "openapp", "testyaml.yaml", "default"};
    apollo_client::apollo_openapi_base openapi_client;
    auto res = openapi_client.getConifgNoProperties(env.base_url_, "DEV", env.appid_, env.cluster_, env.namespace_);
    if (res.has_value()) {
        std::cout << res.value().size() << std::endl;
    }
}

// test for modify yaml config
void test_modify_by_openapi()
{
    test_env env = {"localhost:8070", "openapp", "testyaml.yaml", "default"};
    apollo_client::apollo_openapi_base openapi_client;
    std::string newyaml = "kokdoa,;[;[doakdao";
    std::string user = "apollo";
    std::string comment = "尝试修改";
    auto res = openapi_client.modifyConfigNoProperties(env.base_url_, "DEV",
                                                       env.appid_,
                                                       env.cluster_,
                                                       env.namespace_,
                                                       newyaml.c_str(),
                                                       "apollo",
                                                       false,
                                                       comment,
                                                       user);
}

// test for pushlish yaml file
void test_publish_by_openapi()
{
    test_env env = {"localhost:8070", "openapp", "testyaml.yaml", "default"};
    apollo_client::apollo_openapi_base openapi_client;

    std::string releasetitle = "尝试发布v1";
    std::string user = "apollo";
    std::string releaseComment = "发布评论";

    auto res = openapi_client.publishConfig(env.base_url_,
                                            "DEV",
                                            env.appid_,
                                            env.cluster_,
                                            env.namespace_, releasetitle, user, releaseComment);
}

//test delete yaml config

void test_delete_yaml_config()
{
    test_env env = {"localhost:8070", "openapp", "testyaml.yaml", "default"};
    apollo_client::apollo_openapi_base openapi_client;
    std::string user = "apollo";

    auto res = openapi_client.deleteConfig(env.base_url_,
                                           "DEV",
                                           env.appid_,
                                           env.cluster_,
                                           env.namespace_,
                                           user);
}

//TODO，删除一个yaml配置之后，无法直接再修改配置，需要createIfnotExists配置才可以使用。
int main()
{
    test_modify_by_openapi();
    test_publish_by_openapi();
    return 0;
}