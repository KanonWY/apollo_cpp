#ifndef __APOLLO_BASE_H__
#define __APOLLO_BASE_H__

#include <string>
#include <map>
#include <set>

namespace apollo_client
{
class apollo_base
{
public:
    std::map<std::string, std::string> getConfigNoBufferInner(const std::string &config_server_url,
                                                              const std::string &appid_name,
                                                              const std::string &namespace_name,
                                                              const std::string &cluster_name);

    bool getConfigNoBufferInner(const std::string &config_server_url,
                                const std::string &appid_name,
                                const std::string &namespace_name,
                                const std::string &cluster_name,
                                std::map<std::string, std::string> &output);

    std::string getConfigNoBufferByKeyInner(const std::string &config_server_url,
                                            const std::string &appid_name,
                                            const std::string &namespace_name,
                                            const std::string &cluster_name,
                                            const std::string &key);

    std::map<std::string, std::string> getConfigNoBufferInnerByYAML(const std::string &config_server_url,
                                                                    const std::string &appid_name,
                                                                    const std::string &namespace_name,
                                                                    const std::string &cluster_name);

    virtual ~apollo_base() = default;
};

//This is can change the server config

class apollo_openapi_base
{
public:
    std::set<std::map<std::string, std::string>> getAllAppInfo(const std::string &appIds);

    // cluster about

    // 1. get cluster info

    // 2. create a new cluster

    // ns about

    // config about

    // 1. get config

    // 2. add config

    // 3. modify config

    // 4. delete config

    // 5. pub config
};

} // namespace apollo_client

#endif