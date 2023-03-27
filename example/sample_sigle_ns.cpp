#include <iostream>
#include "apollo_client.h"
#include <signal.h>
#include <atomic>

std::atomic<bool> run{true};

void sigFunc(int num)
{
    std::cout << "signal handler" << std::endl;
    run = false;
}

int main()
{
    {
        ApolloClientSingleNs client{};
        client.init("http://localhost:8080", "cp1", "default", "p1_pram_set");
        client.setCallback([]() {
            std::cout << "参数更新，回调" << std::endl;
        });
        client.turnOnCallback();

        signal(SIGINT, sigFunc);
        signal(SIGTERM, sigFunc);

        while (run) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    return 0;
}