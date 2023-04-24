#include "message.pb.h"
#include "../message.srpc.h"
#include "workflow/WFFacilities.h"

static WFFacilities::WaitGroup wait_group(1);

const static std::string appid = "openapp";
const static std::string test_token = "d4485a2ebe1f516172509635db60e90e2670715458963d44aeab27821b6ea82d";

int main()
{
    apollo_param::EchoRequest req;
    req.set_appid(appid);
    req.set_namespace_("testyaml.yaml");
    req.set_op(::apollo_param::OPERATOR::GET);
    req.set_key("properties/record_online_frame_num/value");

    apollo_param::Example::SRPCClient client("127.0.0.1", 8822);
    client.Echo(&req, [](apollo_param::EchoResponse *resq, srpc::RPCContext *ctx) {
        if (ctx->success()) {
            printf("%s\n", resq->DebugString().c_str());
        } else {
            printf("status[%d], error[%d], errmsg:%s\n", ctx->get_status_code(), ctx->get_error(), ctx->get_errmsg());
        }
    });

    wait_group.wait();
    return 0;
}