#include "../include/apollo_openapi_base.h"

static std::string token = "eafb8c5d6c45c8c11961ee60a74e8f04775596b08efaf4869807060e8af1eca5";
static std::string Address = "http://localhost:8070";

int main()
{
    apollo_client::apollo_ctrl_base base;
    apollo_client::MultiNsConfig config;
    config.SetAppid("openapp").SetAddress(Address);
    base.init(token, config);

    // auto res = base.setConfig("test1/jdiajdi", "dmaodkoadk", "testyaml.yaml");
    auto res = base.addNewConfig("openapp", "testyaml.yaml", "hekklplp/ldoakldop", "ldpaldp");
    if (res) {
        SPDLOG_INFO("suceess");
        base.publishNamespace("testyaml.yaml", "ldpalpd");
    } else {
        SPDLOG_ERROR("err");
    }

    return 0;
}