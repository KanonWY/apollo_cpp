#include "apollo_base_client.h"

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
    apollo_client::AppConfig<apollo_client::MULTI_NAMESPACE> config;
    //    apollo_client::apollo_client_base<apollo_client::MULTI_NAMESPACE, apollo_client::STRING_MAP> cl;
    //    //    cl.init()
    apollo_client::MultiNsConfig ns;
    ns.SetAddress("http://kanon2020.top:8080").SetAppid("SampleApp").AppendNamespace("test_yaml.yaml");
    apollo_client::apollo_client_multi_ns cl;
    cl.init(ns);
    auto res = cl.getConfigByKey("key1");
    std::cout << "res = " << res.c_str() << std::endl;
}

int main()
{
    testMultiNs();
    return 0;
}
