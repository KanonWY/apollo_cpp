#include "../include/apollo_base.h"

/**
 * @brief This file test basic openapi interface.
 */

const static std::string test_token = "d4485a2ebe1f516172509635db60e90e2670715458963d44aeab27821b6ea82d";

/**
 * @brief  1.create a new yaml type ns, 2.modify this ns. 3. update this ns, 4. delete this config.
 */
void test_create_modify_delete()
{
    apollo_client::apollo_openapi_base openapi_client;
    // 1. GET TOKEN
    openapi_client.init(test_token);
    // 2. create new yaml type ns
    openapi_client.createNewNamespace("localhost:8070",
                                      "openapp",
                                      "fulltest",
                                      "yaml",
                                      "apollo",
                                      false,
                                      "创建一个新的ns用于测试");
    // 3. base ns create new config
    openapi_client.createNewConfig("localhost:8070",
                                   "DEV",
                                   "openapp",
                                   "default",
                                   "fulltest.yaml",
                                   "content",
                                   "hello: value",
                                   "apollo",
                                   "修改新创建ns.yaml");
    // 4. publish the new config.
    openapi_client.publishConfig("localhost:8070",
                                 "DEV",
                                 "openapp",
                                 "default",
                                 "fulltest.yaml",
                                 "将新建的发布",
                                 "apollo",
                                 "测试发布");
    // 5. delete new config.
    openapi_client.deleteConfig("localhost:8070",
                                "DEV",
                                "openapp",
                                "default",
                                "fulltest.yaml",
                                "apollo");
    // 6. publish delete info.
    openapi_client.publishConfig("localhost:8070",
                                 "DEV",
                                 "openapp",
                                 "default",
                                 "fulltest.yaml",
                                 "将新建的发布",
                                 "apollo",
                                 "测试发布");
};

//TODO，删除一个yaml配置之后，无法直接再修改配置，需要createIfnotExists配置才可以使用。

/**
 * @brief test for modify namespace.yaml by openapi
 */
void test_modify_by_openapi()
{
    apollo_client::apollo_openapi_base openapi_client;
    openapi_client.init(test_token);
    // 1. create a new ns for test modify
    openapi_client.createNewNamespace("localhost:8070",
                                      "openapp",
                                      "testmodify",
                                      "yaml",
                                      "apollo",
                                      false,
                                      "创建一个新的ns用于测试");
    // 2. base ns create new config
    openapi_client.createNewConfig("localhost:8070",
                                   "DEV",
                                   "openapp",
                                   "default",
                                   "testmodify.yaml",
                                   "content",
                                   "createNewConfig",
                                   "apollo",
                                   "修改新创建ns.yaml");

    // 3. modify the config
    openapi_client.modifyConfigNoProperties("localhost:8070",
                                            "DEV",
                                            "openapp",
                                            "default",
                                            "testmodify.yaml",
                                            "first modify",
                                            "apollo",
                                            false,
                                            "第一次修改",
                                            "");
}

int main()
{
    test_modify_by_openapi();
    return 0;
}