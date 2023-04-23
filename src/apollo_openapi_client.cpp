#include "../include/apollo_openapi_client.h"

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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////// ----->  apollo_openapi_client_single <------ ////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool apollo_openapi_client_single::prepare(const std::string &address,
                                           const std::string &appid,
                                           const std::string &token,
                                           const std::string &env,
                                           const std::string &opperson)
{
    address_ = address;
    appid_ = appid;
    env_ = env;
    op_person_ = opperson;
    init(token);
    //get all ns.
    return true;
}
bool apollo_openapi_client_single::createNewPrivateYamlConfig(const std::string &configname,
                                                              const std::string &content,
                                                              const std::string &comment)
{
    bool re{false};
    re = createNewNamespace(address_,
                            appid_,
                            configname,
                            "yaml",
                            op_person_,
                            false,
                            comment);
    if (!re) {
        return re;
    }
    re = createNewConfig(address_, env_,
                         appid_, cluster_,
                         configname, "content",
                         content, op_person_, comment);
    return re;
}
bool apollo_openapi_client_single::createNewPublicYamlConfig(const std::string &configname,
                                                             const std::string &content,
                                                             const std::string &comment)
{
    return createNewNamespace(address_, appid_,
                              configname, "yaml",
                              op_person_, true,
                              comment);
}
bool apollo_openapi_client_single::modifyYamlConfig(const std::string &configname,
                                                    const std::string &content,
                                                    const std::string &comment)
{
    return modifyConfigNoProperties(address_, env_,
                                    appid_, cluster_,
                                    configname, content,
                                    op_person_, true,
                                    comment, op_person_);
}
bool apollo_openapi_client_single::deleteYamlConfig(const std::string &configname)
{
    return deleteConfig(address_, env_,
                        appid_, cluster_,
                        configname, op_person_);
}
bool apollo_openapi_client_single::publishYamlConfig(const std::string &configname,
                                                     const std::string &releaseTitle,
                                                     const std::string &comment)
{
    return publishConfig(address_, env_,
                         appid_, cluster_,
                         configname, releaseTitle,
                         op_person_, comment);
}
std::string apollo_openapi_client_single::getYamlConfig(const std::string &configname)
{
    return getConfigNoProperties(address_, env_,
                                 appid_, cluster_,
                                 configname);
}

} // namespace apollo_client