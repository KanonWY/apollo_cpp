//
// Created by kanon on 4/3/23.
//

#include "../include/apollo_openapi_client.h"

/**
 * @brief test create new yaml config and pub.
 */
void test_openapi_client_single()
{
    apollo_client::apollo_openapi_client_single client;
    client.prepare("localhost:8070",
                   "openapp",
                   "d4485a2ebe1f516172509635db60e90e2670715458963d44aeab27821b6ea82d");
    client.createNewPrivateYamlConfig("single.yaml", "key1: value1", "参赛规划");
    client.publishYamlConfig("single.yaml", "ceshi", "kcokoc");
}

/**
 * @brief test modify
 * @param content
 */
void test_modify(const std::string &content = "")
{
    std::string ct = "dkoadkoadkoa";
    if (!content.empty()) {
        ct = content;
    }
    apollo_client::apollo_openapi_client_single client;
    client.prepare("localhost:8070",
                   "openapp",
                   "d4485a2ebe1f516172509635db60e90e2670715458963d44aeab27821b6ea82d");
    client.modifyYamlConfig("single.yaml", ct, "修改");
    client.publishYamlConfig("single.yaml", "ced1d1", "doakdoadk");
}

int main()
{
    test_modify("第二次修改");
    return 0;
}