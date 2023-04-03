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
} // namespace apollo_client

#endif //APOLLO_CPP_CLIENT_APOLLO_OPENAPI_CLIENT_H
