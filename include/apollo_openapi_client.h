#ifndef APOLLO_CPP_CLIENT_APOLLO_OPENAPI_CLIENT_H
#define APOLLO_CPP_CLIENT_APOLLO_OPENAPI_CLIENT_H
#include "apollo_base.h"
#include <map>
#include <string>

namespace apollo_client
{
/**
 * @brief apollo openapi client which can control config server's params.
 */
class apollo_openapi_client : public apollo_openapi_base
{
public:
    /**
     * @brief add a token of a appid.(will overload older token)
     * @param appid
     * @param tokens
     */
    void addTokens(const std::string &appid, const std::string &tokens);

    /**
     * @brief add multi token of appid.
     * @param appidTokens
     */
    void addTokens(const std::map<std::string, std::string> &appidTokens);

private:
    std::map<std::string, std::string> appid_tokens_;
};

/**
 * @brief this class only control one appid.
 */
class apollo_openapi_client_single : public apollo_openapi_base
{
public:
    /**
     * @brief prepare to fill env info
     * @param address
     * @param appid
     * @param token
     * @param dev
     */
    bool prepare(const std::string &address,
                 const std::string &appid,
                 const std::string &token,
                 const std::string &dev = "DEV",
                 const std::string &opperson = "apollo");

    /**
     * @brief create a new private yaml config
     * @param configname
     * @param content
     * @return
     */
    bool createNewPrivateYamlConfig(const std::string &configname,
                                    const std::string &content,
                                    const std::string &comment);

    /**
     * @brief create a new public yaml config
     * @param configname
     * @param content
     * @param comment
     * @return
     */
    bool createNewPublicYamlConfig(const std::string &configname,
                                   const std::string &content,
                                   const std::string &comment);

    /**
     * @brief modify yaml config.
     * @param configname
     * @param content
     * @return
     */
    bool modifyYamlConfig(const std::string &configname,
                          const std::string &content,
                          const std::string &comment);

    /**
     * @brief delete yaml config.
     * @param configname
     * @return
     */
    bool deleteYamlConfig(const std::string &configname);

    /**
     * @brief publish the yaml config.
     * @param configname
     * @return
     */
    bool publishYamlConfig(const std::string &configname,
                           const std::string &releaseTitle,
                           const std::string &comment);

    /**
     * @brief get yaml config string.
     * @param configname
     * @return
     */
    std::string getYamlConfig(const std::string &configname);

private:
    std::string appid_;
    std::string cluster_{"default"};
    std::string env_{"DEV"};
    std::string address_;
    std::string op_person_{"apollo"};
    std::vector<std::string> namespaces_;
};

} // namespace apollo_client

#endif //APOLLO_CPP_CLIENT_APOLLO_OPENAPI_CLIENT_H
