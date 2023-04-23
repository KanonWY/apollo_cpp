#ifndef __YAML_CODE_TEMPLATE_H__
#define __YAML_CODE_TEMPLATE_H__

//ProcessId: process
//version: 1
//testmap:
//  app: hello
//  mage: abc
//testvector:
//  - hello
//  - world
//  - dkoakdoa
//  - kdoakdo
//testvectormap:
//  - map1key1: value1
//    map1key2: value2
//  - map2key1: value1
//    map2key2: value2

struct testMap
{
    std::string app;
    std::string image;
};

struct NsStore
{
    std::string appId;
    std::string clusterName;
    std::string namespaceName;
    std::string comment;
    std::string format;
    bool isPublic;
    std::vector<std::map<std::string, std::string>> items;
    std::string dataChangeCreatedBy;
    std::string dataChangeLastModifiedBy;
    std::string dataChangeCreatedTime;
    std::string dataChangeLastModifiedTime;
};
struct AppAllNamespace
{
    std::vector<NsStore> vec;
};

namespace YAML
{
template <>
struct convert<testMap>
{
    static Node encode(const testMap &rhs)
    {
        Node node;
        node.push_back(rhs.app);
        node.push_back(rhs.image);
        return node;
    }

    static bool decode(const Node &node, testMap &rhs)
    {
        std::cout << node.Type() << std::endl;
        rhs.app = node["app"].as<std::string>();
        rhs.image = node["image"].as<std::string>();
        return true;
    }
};

template <>
struct convert<NsStore>
{
    static Node encode(const NsStore &store)
    {
        Node node;
        node["appId"] = store.appId;
        node["clusterName"] = store.clusterName;
        node["namespaceName"] = store.namespaceName;
        node["comment"] = store.comment;
        node["format"] = store.format;
        node["isPublic"] = store.isPublic;
        node["dataChangeCreatedBy"] = store.dataChangeCreatedBy;
        node["dataChangeLastModifiedBy"] = store.dataChangeLastModifiedBy;
        node["dataChangeCreatedTime"] = store.dataChangeCreatedTime;
        node["dataChangeLastModifiedTime"] = store.dataChangeLastModifiedTime;

        Node items_node;
        for (const auto &item : store.items) {
            Node item_node;
            for (const auto &kv : item) {
                item_node[kv.first] = kv.second;
            }
            items_node.push_back(item_node);
        }
        node["items"] = items_node;

        return node;
    }

    static bool decode(const Node &node, NsStore &store)
    {
        if (!node.IsMap()) {
            return false;
        }

        store.appId = node["appId"].as<std::string>();
        store.clusterName = node["clusterName"].as<std::string>();
        store.namespaceName = node["namespaceName"].as<std::string>();
        store.comment = node["comment"].as<std::string>();
        store.format = node["format"].as<std::string>();
        store.isPublic = node["isPublic"].as<bool>();
        store.dataChangeCreatedBy = node["dataChangeCreatedBy"].as<std::string>();
        store.dataChangeLastModifiedBy = node["dataChangeLastModifiedBy"].as<std::string>();
        store.dataChangeCreatedTime = node["dataChangeCreatedTime"].as<std::string>();
        store.dataChangeLastModifiedTime = node["dataChangeLastModifiedTime"].as<std::string>();

        const auto &items_node = node["items"];
        store.items.reserve(items_node.size());
        for (const auto &item_node : items_node) {
            std::map<std::string, std::string> item;
            for (const auto &kv : item_node) {
                item[kv.first.as<std::string>()] = kv.second.as<std::string>();
            }
            store.items.push_back(std::move(item));
        }

        return true;
    }
};

template <>
struct convert<AppAllNamespace>
{
    static Node encode(const AppAllNamespace &app_all_ns)
    {
        Node node;
        for (const auto &ns_store : app_all_ns.vec) {
            node.push_back(ns_store);
        }
        return node;
    }

    static bool decode(const Node &node, AppAllNamespace &app_all_ns)
    {
        if (!node.IsSequence()) {
            return false;
        }

        app_all_ns.vec.clear();
        for (const auto &ns_node : node) {
            NsStore ns_store;
            ns_node.as<NsStore>(ns_store);
            app_all_ns.vec.push_back(ns_store);
        }
        return true;
    }
};

} // namespace YAML

#endif