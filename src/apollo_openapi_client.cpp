#include "apollo_openapi_client.h"

namespace apollo_client
{
void apollo_openapi_client::addTokens(const std::string &appid, const std::string &tokens)
{
    appid_tokens_[appid] = tokens;
}
void apollo_openapi_client::addTokens(const std::map<std::string, std::string> &appidTokens)
{
    for (const auto &item : appidTokens) {
        appid_tokens_[item.first] = item.second;
    }
}
} // namespace apollo_client