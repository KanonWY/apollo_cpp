#include "apollo_client_proxy.h"
#include <iostream>
#include <map>
#include <string>
#include "basemessage.h"
#define PORT 12345



//std::vector<char> pack_write(const std::string &response)
//{
//    std::cout << "package_write" << std::endl;
//    STcpHeader tcp_header;
//    const size_t psize = response.size();
//    tcp_header.psize_n = htonl(static_cast<uint32_t>(psize));
//    std::vector<char> packed_response(sizeof(tcp_header) + psize);
//    memcpy(packed_response.data(), &tcp_header, sizeof(tcp_header));
//    memcpy(packed_response.data() + sizeof(tcp_header), response.data(),
//           psize);
//    return packed_response;
//}


void unpack_request(std::string& request)
{
    char* data = new char[request.size()];
    STcpHeader *tcp_header = reinterpret_cast<STcpHeader*>(data);
}




int reqFunc(const std::string &request, std::string &response)
{
    //TODO: deal with tcp stream contact
    EASY_TCP::MsgContent content;
    content.ParseFromString(request);
    std::cout << "content version = " << content.version() << std::endl;
    auto version = content.version();
    auto cmd =content.cmd();
    auto inner_content_str = content.content();

    EASY_TCP::RequestContent requestContent;
    requestContent.ParseFromString(inner_content_str);
    auto ts = requestContent.timestamp();

    auto appid = requestContent.appid();

    std::vector<std::string> ns_v;
    for(int i = 0; i < requestContent.namespace_vec_size(); ++i)
    {
        ns_v.push_back(requestContent.namespace_vec(i));
    }
    std::cout << "appid = " << appid.c_str() << std::endl;
    for(const auto& item : ns_v)
    {
        std::cout << "ns = " << item.c_str() << std::endl;
    }

    //package response.

    EASY_TCP::ResponseContent responseContent;
    responseContent.set_timestamp(getTimestampNow());
    responseContent.set_ack(2200);
    responseContent.set_appid(appid);
    auto ma = responseContent.mutable_namespace_config_map();
    ma->insert({"key1","value1"});
    ma->insert({"key2","value2"});
    std::cout << "map size = " << responseContent.namespace_config_map().size() << std::endl;

    std::string sendstr;
    responseContent.SerializeToString(&sendstr);

    EASY_TCP::MsgContent sendmsg;
    sendmsg.set_version(version);
    sendmsg.set_cmd(EASY_TCP::CMD_TYPE::RESPONSE);
    sendmsg.set_content(sendstr.c_str());
    sendstr.clear();
    sendmsg.SerializeToString(&sendstr);

    response = sendstr;

    std::cout << "response size = " << response.size() << std::endl;
    return 0;
}

void eventFunc(Server_Event event, const std::string &message)
{
    std::cout << "eventFunc" << std::endl;
}

int main()
{
    easy_tcp::apollo_client_proxy proxy_server;

    proxy_server.start([](auto && PH1, auto && PH2) { return reqFunc(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2)); },
                 [](auto && PH1, auto && PH2) { return eventFunc(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2)); }, PORT);

    while (true) {
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }

    return 0;
}
