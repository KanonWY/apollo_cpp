#include "apollo_param_base.h"

namespace apollo_param
{

bool param_server_config::isValid() const
{
    {
        if (!apollo_openapi_ip_.empty() && secret_.empty()) {
            return false;
        }
        if (appid_.empty()) {
            return false;
        }
        return true;
    }
}
void param_server_config::fillClientConfig(apollo_client::MultiNsConfig &config) const
{
    config.appid_ = appid_;
    config.secret_ = secret_;
    if (!apollo_config_ip_.empty()) {
        config.address_ = apollo_config_ip_;
    } else {
        config.address_ = apollo_openapi_ip_;
    }
    if (env_.empty()) {
        config.env_ = env_;
    }
    if (!cluster_.empty()) {
        config.cluster_ = cluster_;
    }
    if (!namespaces_.empty()) {
        config.namespace_ = namespaces_;
    }
}
}; // namespace apollo_param