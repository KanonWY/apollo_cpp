#ifndef APOLLO_CPP_SRPC_BACKSERVER_H
#define APOLLO_CPP_SRPC_BACKSERVER_H
#include "apollo_param_base.h"
#include "message.srpc.h"

namespace apollo_param
{

class srpcServer_;

class ExampleServiceImpl : public Example::Service
{
public:
    void Echo(EchoRequest *request, EchoResponse *response, srpc::RPCContext *ctx) override;

    void setRawPtr(apollo_param::srpcServer_ *sp)
    {
        imp_raw_ptr_ = sp;
    }

private:
    apollo_param::srpcServer_ *imp_raw_ptr_{nullptr};
};

class srpcServer_ : public Base_Server_1<srpcServer_>
{
public:
    bool initImp(short port, const std::string &address)
    {
        imp_ = new ExampleServiceImpl;
        imp_->setRawPtr(this);
        server_.add_service(imp_);
        return server_.start(8822);
    }

    ~srpcServer_() override
    {
        if (imp_ != nullptr) {
            delete imp_;
            imp_ = nullptr;
        }
        server_.stop();
    }

private:
    srpc::SRPCServer server_{};
    ExampleServiceImpl *imp_{nullptr};
};

using Server_Query_sp = std::shared_ptr<Base_Server_1<srpcServer_>>;
using Server_Query = srpcServer_;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SsrpcServer_;

class SServiceImpl : public Example::Service
{
public:
    void Echo(EchoRequest *request, EchoResponse *response, srpc::RPCContext *ctx) override;

    void setRawPtr(apollo_param::SsrpcServer_ *sp)
    {
        imp_raw_ptr_ = sp;
    }

private:
    apollo_param::SsrpcServer_ *imp_raw_ptr_{nullptr};
};

class SsrpcServer_ : public Base_Server_2<SsrpcServer_>
{
public:
    bool initImp(short port, const std::string &address)
    {
        imp_ = new SServiceImpl;
        imp_->setRawPtr(this);
        server_.add_service(imp_);
        return server_.start(8822);
    }

    ~SsrpcServer_() override
    {
        if (imp_ != nullptr) {
            delete imp_;
            imp_ = nullptr;
        }
        server_.stop();
    }

private:
    srpc::SRPCServer server_{};
    SServiceImpl *imp_{nullptr};
};

using Server_CTRL_sp = std::shared_ptr<Base_Server_2<SsrpcServer_>>;
using Server_CTRL = SsrpcServer_;

}; // namespace apollo_param

#endif //APOLLO_CPP_SRPC_BACKSERVER_H
