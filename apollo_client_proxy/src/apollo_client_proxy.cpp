#include "apollo_client_proxy.h"

#include <utility>

namespace easy_tcp
{
apollo_client_proxy::apollo_client_proxy()
{
    //构建假数据
    std::map<std::string, std::string> ns_data_1 = {{"test_yaml.yaml", "key:value"}};
    test_data_map.insert({"SampleApp", ns_data_1});
}
void apollo_client_proxy::start(RequestCallbackT request_callback_,
                                EventCallbackT event_callback_, short port){
    tcp_server_.Create();
    tcp_server_.Start(std::move(request_callback_),std::move(event_callback_),port);
};
} // namespace easy_tcp