//
// Created by kanon on 4/10/23.
//

#ifndef APOLLO_CPP_CLIENT_APOLLO_CLIENT_BASE_H
#define APOLLO_CPP_CLIENT_APOLLO_CLIENT_BASE_H

#include <string>

namespace apollo_base
{

struct env_base
{
    std::string address;
    std::string appid;
    std::string env;
    std::string cluster;
};

class apollo_client_base;
} // namespace apollo_base

#endif //APOLLO_CPP_CLIENT_APOLLO_CLIENT_BASE_H
