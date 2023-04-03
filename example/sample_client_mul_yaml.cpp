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
    auto v = client.getAllNamespaces();
    int i = 0;
    for (const auto &item : v) {
        SPDLOG_INFO("namespace_{} = {}", i++, item);
    }
    while (run) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if (triggle) {
            auto node = client.getNsNameConfigNode("test_yaml22.yaml");

            if (node.Type() == YAML::NodeType::Scalar) {
                YAML::Node nd = YAML::Load(node.as<std::string>().c_str());
                SPDLOG_INFO("msg = {}", node.as<std::string>().c_str());
            }
            triggle = false;
        }
    }
    return 0;
}