#include "../srpc_backserver.h"
#include <csignal>
#include "workflow/WFFacilities.h"

static WFFacilities::WaitGroup wait_group(1);
static void sig_handler(int signo)
{
    wait_group.done();
}

const static std::string appid = "openapp";
const static std::string test_token = "d4485a2ebe1f516172509635db60e90e2670715458963d44aeab27821b6ea82d";

int main2()
{
    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);

    srpc::SRPCServer server_tcp;
    apollo_param::ExampleServiceImpl server;

    server_tcp.add_service(&server);

    if (server_tcp.start(8822) == 0) {
        wait_group.wait();
        server_tcp.stop();
    } else {
        std::cout << "error" << std::endl;
    }
    return 0;
}

int main3()
{
    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);

    auto *server_tcp = new srpc::SRPCServer;
    auto *server = new apollo_param::ExampleServiceImpl;

    server_tcp->add_service(server);

    if (server_tcp->start(8822) == 0) {
        wait_group.wait();
        server_tcp->stop();
    } else {
        std::cout << "error" << std::endl;
    }
    return 0;
}

int main()
{
    apollo_param::param_server_config config;
    config.setport_value(8822).setserver_ip_value("localhost");
    config.setappid_value(appid).setsecret_value(test_token).setapollo_config_ip_value("http://localhost:8080");
    config.setnamespace_value("testyaml.yaml");

    //    apollo_param::Base_Server<apollo_param::srpcServer_> *sp = new apollo_param::srpcServer_;

    apollo_param::Server_Query_sp sp = std::make_shared<apollo_param::Server_Query>();

    if (sp->init(config) == 0) {
        std::cout << "start success" << std::endl;
        wait_group.wait();
    } else {
        std::cout << "error" << std::endl;
    }
    return 0;
}