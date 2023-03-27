#include <spdlog/spdlog.h>
#include "apollo_client_mulns.h"
#include <csignal>

bool run = true;
bool flag = false;

void sigFunc(int sig_num)
{
    run = false;
}

int main()
{
    {
        apollo_client::apollo_mulns_client client;
        client.init("http://localhost:8080", "cp1", "default", {"p1_pram_set", "p2_pram_set", "type1_public_config"});

        client.setCallback([]() {
            flag = true;
        });
        client.turnonCallback();

        while (run) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            if (flag) {
                auto res = client.getConfigureByNs("type1_public_config");
                for (const auto &item : res) {
                    SPDLOG_INFO("key = {}, value = {}", item.first.c_str(), item.second.c_str());
                }
                flag = false;
            }
        }
    }
    return 0;
}