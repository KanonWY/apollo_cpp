#include "srpc_backserver.h"

namespace apollo_param
{

void ExampleServiceImpl::Echo(EchoRequest *request, EchoResponse *response, srpc::RPCContext *ctx)
{
    printf("get_req:%s\n", request->DebugString().c_str());
    auto val = imp_raw_ptr_->getStringConfig(request->key());
    response->set_value(val);
}

}; // namespace apollo_param
