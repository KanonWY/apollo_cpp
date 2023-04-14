#include <apollo_openapi_base.h>
#include <gflags/gflags.h>

DEFINE_string(cmd, "", "which to: gv/gkn/gkall/del/pub");
DEFINE_string(ns, "", "namespaceName");
DEFINE_string(key, "", "keyName not include appid/namespace prefix ");
DEFINE_string(appId, "", "appId want to use");
DEFINE_string(address, "http://localhost:8070", "server address");
DEFINE_string(token, "eafb8c5d6c45c8c11961ee60a74e8f04775596b08efaf4869807060e8af1eca5", "open api need a token");
DEFINE_string(publishtitle, "publish", "publish title");
DEFINE_string(publishuser, "apollo", "who publish this!");
DEFINE_string(publishcomment, "comment", "add comment about this publish");

std::string strNode(YAML::Node &node)
{
    std::ostringstream ss;
    ss << node;
    return ss.str();
}

int main(int argc, char *argv[])
{
    gflags::SetUsageMessage("Usage: -");
    gflags::SetVersionString("version 1.0");
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    apollo_client::apollo_ctrl_base base;
    apollo_client::MultiNsConfig config;
    if (FLAGS_appId.empty()) {
        std::cout << "Must input appId" << std::endl;
        goto fin;
    }
    if (FLAGS_cmd.empty()) {
        std::cout << "Must input cmd" << std::endl;
        goto fin;
    }
    config.SetAddress(FLAGS_address).SetAppid(FLAGS_appId);
    base.init(FLAGS_token, config);

    if (!strcmp(FLAGS_cmd.c_str(), "gkn")) {
        //get all keys in a namespace
        if (FLAGS_ns.empty()) {
            std::cout << "namespace is empty()" << std::endl;
            goto fin;
        } else {
            auto res = base.getNamespaceAllKey(FLAGS_ns);
            if (res.has_value()) {
                for (const auto &item : res.value()) {
                    std::cout << item << std::endl;
                }
            } else {
                std::cout << FLAGS_ns << " have no keys" << std::endl;
            }
        }
    } else if (!strcmp(FLAGS_cmd.c_str(), "gkall")) {
        //get all keys
        auto res = base.getAllKey();
        if (res.has_value()) {
            for (const auto &item : res.value()) {
                std::cout << item << std::endl;
            }
        } else {
            std::cout << "This appid have no any keys!" << std::endl;
        }
    } else if (!strcmp(FLAGS_cmd.c_str(), "gv")) {
        if (FLAGS_key.empty() || FLAGS_ns.empty()) {
            std::cout << "gv command must use key and ns!" << std::endl;
            goto fin;
        } else {
            auto res = base.getConfig(FLAGS_appId, FLAGS_ns, FLAGS_key);
            if (res.has_value()) {
                std::cout << strNode(res.value()) << std::endl;
            } else {
                std::cout << FLAGS_key << " is not exist!" << std::endl;
            }
        }
        //get specail key value
    } else if (!strcmp(FLAGS_cmd.c_str(), "del")) {
        // delete a special key
    } else if (!strcmp(FLAGS_cmd.c_str(), "pub")) {
        if (FLAGS_publishtitle.empty()) {
            std::cout << "Must input publishtitle" << std::endl;
            goto fin;
        }
        if (base.publishNamespace(FLAGS_ns, FLAGS_publishtitle, FLAGS_publishuser, FLAGS_publishcomment)) {
            std::cout << "publish " << FLAGS_ns << " success!" << std::endl;
        } else {
            std::cout << "publish " << FLAGS_ns << " error!" << std::endl;
        }

    } else {
        std::cout << "unknow cmd Type" << std::endl;
    }
fin:
    gflags::ShutDownCommandLineFlags();
    return 0;
}