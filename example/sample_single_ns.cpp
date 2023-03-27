#include "apollo_client_singlens.h"
#include <csignal>
#include <spdlog/spdlog.h>

bool run = true;
bool flag = false;

void sigFunc(int num)
{
    std::cout << "signal handler" << std::endl;
    run = false;
}

int main()
{
    {
        apollo_client::apollo_sgns_client client{};
        client.init("http://localhost:8080", "cp1", "default", "p2_pram_set");
        client.setCallback([]() {
            flag = true;
        });
        client.turnonCallback();

        while (run) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            if (flag) {
                client.dumpConfig();
                flag = false;
            }
        }
    }
    return 0;
}