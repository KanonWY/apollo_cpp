#include "../srpc_backserver.h"
#include <csignal>
#include "workflow/WFFacilities.h"

static WFFacilities::WaitGroup wait_group(1);
static void sig_handler(int signo)
{
    wait_group.done();
}

const static std::string appid = "openapp";
const static std::string test_token = "489df4c4aaea17c37b1f8fd888a968fb0c4685bedc1d9f2555f1a016c08b2e9f";

int main()
{
    apollo_param::param_server_config config;
    config.setport_value(8822).setserver_ip_value("localhost").setenv_value("DEV");
    config.setappid_value(appid).setsecret_value(test_token).setapollo_config_ip_value("http://localhost:8070");
    config.setnamespace_value("testyaml.yaml");

    //    apollo_param::Base_Server<apollo_param::srpcServer_> *sp = new apollo_param::srpcServer_;

    apollo_param::Server_CTRL_sp sp = std::make_shared<apollo_param::Server_CTRL>();

    if (sp->init(config) == 0) {
        std::cout << "start success" << std::endl;
        wait_group.wait();
    } else {
        std::cout << "error" << std::endl;
    }
    return 0;
}