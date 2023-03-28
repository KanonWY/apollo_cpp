#include "apollo_client_mul_yaml.h"
#include <spdlog/spdlog.h>

bool run{true};
bool triggle{false};

int main()
{
    apollo_client::apollo_mul_yaml_client client;
    client.init("http://localhost:8080", "SampleApp", "default", {"test_yaml.yaml", "test_yaml22.yaml"});
    client.setCallback([]() {
        triggle = true;
    });
    client.turnonCallback();
    while (run) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if(triggle)
        {
            auto node = client.getNsNameConfigNode("test_yaml.yaml");
            ;
//            SPDLOG_INFO("node type = {}", node.as<std::string>());
            YAML::Node nd = YAML::Load(node.as<std::string>());
            if(nd["version"])
            {
                SPDLOG_INFO("version = {}", nd["version"].as<std::string>());
            }
            triggle = false;
        }
    }
    return 0;
}