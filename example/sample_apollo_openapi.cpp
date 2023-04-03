#include "apollo_base.h"
#include "sample_base.h"

/**
 * @brief test get config by openapi
 */
void test_get_config_by_openapi()
{
    test_env env = {"localhost:8070", "openapp", "testyaml.yaml", "default"};
    apollo_client::apollo_openapi_base openapi_client;
    openapi_client.init("d4485a2ebe1f516172509635db60e90e2670715458963d44aeab27821b6ea82d");
    auto res = openapi_client.getConfigNoProperties(env.base_url_, "DEV", env.appid_, env.cluster_, env.namespace_);
    if (res.has_value()) {
        std::cout << res.value().size() << std::endl;
    }
}

/**
 * @brief test for modify namespacename.yaml by openapi
 */
void test_modify_by_openapi()
{
    test_env env = {"localhost:8070", "openapp", "testyaml.yaml", "default"};
    apollo_client::apollo_openapi_base openapi_client;
    openapi_client.init("d4485a2ebe1f516172509635db60e90e2670715458963d44aeab27821b6ea82d");
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

/**
 * @brief test for publish namespacename.yaml by openapi.
 */
void test_publish_by_openapi()
{
    test_env env = {"localhost:8070", "openapp", "testyaml.yaml", "default"};
    apollo_client::apollo_openapi_base openapi_client;
    openapi_client.init("d4485a2ebe1f516172509635db60e90e2670715458963d44aeab27821b6ea82d");

    std::string releasetitle = "尝试发布v1";
    std::string user = "apollo";
    std::string releaseComment = "发布评论";

    auto res = openapi_client.publishConfig(env.base_url_,
                                            "DEV",
                                            env.appid_,
                                            env.cluster_,
                                            env.namespace_, releasetitle, user, releaseComment);
}

/**
 * @brief test for delete a namespace.yaml by open api
 */
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

//0dff7daa487fbb5b9644b05902cf2a7ff1b2df48908dd0c42c4d9757e2dd10ce
//TODO，删除一个yaml配置之后，无法直接再修改配置，需要createIfnotExists配置才可以使用。

void test_create_namespace()
{
    apollo_client::apollo_openapi_base openapi_client;
    openapi_client.init("d4485a2ebe1f516172509635db60e90e2670715458963d44aeab27821b6ea82d");
    openapi_client.createNewNamespace("localhost:8070", "openapp", "new_create", "yaml", "apollo",
                                      false, "创建public的yaml配置");
}

void test_create_new_config()
{
    apollo_client::apollo_openapi_base openapi_client;
    openapi_client.init("d4485a2ebe1f516172509635db60e90e2670715458963d44aeab27821b6ea82d");
    openapi_client.createNewConfig("localhost:8070",
                                   "DEV",
                                   "openapp",
                                   "default",
                                   "buf.yaml",
                                   "content",
                                   "key1: value2",
                                   "apollo",
                                   "茶ungjain");
}

int main()
{
    test_create_new_config();
    return 0;
}