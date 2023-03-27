#include <spdlog/spdlog.h>
#include "apollo_client_mulns.h"
#include <csignal>

bool run = true;
bool flag = false;

void sigFunc(int sig_num)
{
    run = false;
}

/**         
 *          explain:         
 * 
 *          1、cp1 app have two private namespace: p1_pram_set  p2_pram_set
 *          
 *          2、pubconfig app have multi public namespace(use pubconfig as the public config appid)
 * */

/**
 *                --- -- -- - cp1(APP) -- -- --  --
 *                           /                    \  
 *               /            \                   \
 *              /             \                    \
 *             /              \                     \
 *            /                \                      \
 *           /                  \                       \
 * 
 *    p1_pram_set(ns1)      p2_pram_set(ns2)             ns3...
 *          |                       |
 *          | have                  | have
 *         \|/                     \|/
 *     [key1:value1]           [key1:value1]     
 *     [key2:value2]           [key2:value2]
 *      ......                     ......
 *          |                         |
 *          |                         |
 *          |                         |
 *          |used by                  | used by
 *          |                         |
 *          |                         |
 *         \|/                       \|/
 *      process_1                process_2
 *          __                     __ 
 *         /\                       /\
 *           \                     /
 *            \                   /
 *             \                 /
 *              \               /
 *               \  can be use /
 *                \           /
 *                 \         /                          |
 *                  \       /                           |  
 *                   \     /                            | can be use
 *                    \   /                             |
 *                     \ /                              |
 * 
 *                  [key1: value1]                [key1: value1]
 *                  [key2: value2]                [key2: value2]
 *                  ......                          ......
 *                     /|\                           /|\
 *                      |  have                       | have
 *                      |                            |
 *                      |                           |
 * 
 *          type1_public_config(ns1)      type2_public_config(ns2)     
 *                   __                    __
 *                  |\                      /\
 *                    \                    /
 *                     \                  /                 
 *                      \                / 
 *          
 *                        pubconfig(public APP)
 * 
 * 
 * */

void dumpStringMap(const std::map<std::string, std::string> &m)
{
    for (const auto &item : m) {
        SPDLOG_INFO("key = {}, value = {}", item.first.c_str(), item.second.c_str());
    }
}

int main()
{
    {
        apollo_client::apollo_mulns_client client;
        //appid = cp1
        // clustername = defaul;
        // cp1 appid have two ns: 1、p1_pram_set;2、p2_pram_set
        // cp1 need to use pubconfig app: type1_public_config

        client.init("http://localhost:8080", "cp1", "default", {"p1_pram_set", "p2_pram_set", "type1_public_config"});

        client.setCallback([]() {
            // when config changed, it'll triggle flag change.
            flag = true;
        });
        client.turnonCallback();

        while (run) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            // triggle change print the all config
            if (flag) {
                auto res = client.getConfigureByNs("p1_pram_set");
                dumpStringMap(res);
                res = client.getConfigureByNs("p2_pram_set");
                dumpStringMap(res);
                res = client.getConfigureByNs("type1_public_config");
                dumpStringMap(res);
                flag = false;
            }
        }
    }
    return 0;
}