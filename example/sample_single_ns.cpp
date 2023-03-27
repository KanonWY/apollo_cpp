#include "apollo_client_singlens.h"
#include <csignal>
#include <spdlog/spdlog.h>

std::atomic<bool> run{true};

void sigFunc(int num)
{
    std::cout << "signal handler" << std::endl;
    run = false;
}

int main()
{
    {
        apollo_client::apollo_sgns_client client{};
        client.init("http://localhost:8080", "cp1", "default", "p1_pram_set");
        client.setCallback([]() {
            SPDLOG_INFO("test callback");
        });
        client.turnOnCallback();

        signal(SIGINT, sigFunc);

        while (run) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    return 0;
}