#include "srpc_backserver.h"

namespace apollo_param
{

void ExampleServiceImpl::Echo(EchoRequest *request, EchoResponse *response, srpc::RPCContext *ctx)
{
    printf("get_req:%s\n", request->DebugString().c_str());
    auto val = imp_raw_ptr_->getStringConfig(request->key());
    response->set_value(val);
}

void SServiceImpl::Echo(EchoRequest *request, EchoResponse *response, srpc::RPCContext *ctx)
{
    printf("get_req:%s\n", request->DebugString().c_str());
    auto val = imp_raw_ptr_->getStringConfig(request->appid(), request->namespace_(), request->key());
    if (val.has_value()) {
        response->set_value(val.value());
    }
}
}; // namespace apollo_param
