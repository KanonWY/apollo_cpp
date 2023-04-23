#include "../include/apollo_base_client.h"

void testAppConfig()
{
    apollo_client::MultiNsConfig config;
    config.AppendNamespace("hell.yaml");
    config.AppendNamespace("jejoko");
    config.ClearNamespace();
    config.dumpInfo();
    config.SetNamespace({"hhelo", "lopdlpa"});
    config.dumpInfo();
}

void testMultiNs()
{
    apollo_client::MultiNsConfig ns;
    ns.SetAddress("http://localhost:8080").SetAppid("SampleApp").AppendNamespace("test_yaml.yaml").AppendNamespace("test_xml.xml");
    apollo_client::apollo_client_multi_ns cl;
    cl.init(ns);
    auto res = cl.getConfigByKey("key1");
    std::cout << "res = " << res.c_str() << std::endl;

    while (true) {
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
}

void testMultiNs_Node()
{
    apollo_client::MultiNsConfig config;
    config.SetAddress("http://localhost:8080").SetAppid("SampleApp").AppendNamespace("test_yaml.yaml").AppendNamespace("test_xml.xml");
    apollo_client::apollo_client_multi_ns_node cl;
    cl.init(config);
    auto res = cl.getConfigByKey("test_yaml.yaml/name");
    if (res.IsScalar()) {
        SPDLOG_INFO("value = {}", res.as<std::string>());
    }
    while (true) {
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
}

int main()
{
    testMultiNs_Node();
    return 0;
}
