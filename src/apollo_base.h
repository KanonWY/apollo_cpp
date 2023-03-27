#ifndef __APOLLO_BASE_H__
#define __APOLLO_BASE_H__

#include <string>
#include <map>


namespace apollo_client
{
class apollo_base
{
public:
    std::map<std::string, std::string> getConfigNoBufferInner(const std::string &config_server_url,
                                                              const std::string &appidName,
                                                              const std::string &namespaceName,
                                                              const std::string &clusterName);

    bool getConfigNoBufferInner(const std::string &config_server_url,
                                const std::string &appidName,
                                const std::string &namespaceName,
                                const std::string &clusterName,
                                std::map<std::string, std::string> &output);

    std::string getConfigNoBufferByKeyInner(const std::string &config_server_url,
                                            const std::string &appidName,
                                            const std::string &namespaceName,
                                            const std::string &clusterName,
                                            const std::string &key);

    virtual ~apollo_base() = default;
};
}

#endif