#include <spdlog/spdlog.h>
#include "apollo_client_mulns.h"
#include <csignal>

bool run = true;

void sigFunc(int sig_num)
{
    run = false;
}

int main()
{
    {
        apollo_client::apollo_mulns_client client;
        client.init("http://localhost:8080", "cp1", "default", {"p1_pram_set", "p2_pram_set"});
        client.setCallback([]() {
            SPDLOG_INFO("update now-------------->");
        });
        client.turnonCallback();

        while (run) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    return 0;
}