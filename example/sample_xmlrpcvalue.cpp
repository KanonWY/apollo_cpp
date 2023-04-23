#include "../include/param.h"
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>

void testSetInt_getInt()
{
    apollo_client::RosStoreContainer container;
    int value = 0;
    container.set("keyint", 29099);
    container.get("keyint", value);
    std::cout << "value = " << value << std::endl;
}

void testSetDouble_getDouble()
{
    apollo_client::RosStoreContainer container;
    double value = 0.0;
    container.set("keydouble", 111.099);
    container.get("keydouble", value);
    std::cout << "value = " << value << std::endl;
}

void testSetString_getString()
{
    apollo_client::RosStoreContainer container;
    std::string value{};
    container.set("keystring", "stringldcaldpalpd");
    container.get("keystring", value);
    std::cout << "value = " << value << std::endl;
}

void testSetVector_getVector()
{
    apollo_client::RosStoreContainer container;

    std::vector<std::string> vstring{"hello", "keoko"};
    container.set("key", vstring);
    std::vector<std::string> store;
    container.get("key", store);
    for (const auto &item : store) {
        std::cout << item << " ";
    }
}

void testSetMap_getMap()
{
    apollo_client::RosStoreContainer container;
    std::map<std::string, int> input{{"ldpalpdlap", 222}};
    container.set("keymap", input);
    std::map<std::string, int> out;
    container.get("keymap", out);
    for (const auto &item : out) {
        std::cout << item.first << " " << item.second << std::endl;
    }
}

void test_complict_type()
{
    apollo_client::RosStoreContainer container;
    std::map<std::string, apollo_client::XmlRpcValue> input;
    input.insert({"hello", "ll"});
    container.set("key", input);
    std::map<std::string, apollo_client::XmlRpcValue> out;
}

//how to use XmlRpcValue to get store yaml

#define YAML_FILE "/home/kanon/Downloads/key_value/example.yaml"

//Undefined, Null, Scalar, Sequence, Map

void parseYamlNode2(const YAML::Node &node, std::map<std::string, std::string> &result, const std::string &prefix)
{
    if (node.IsScalar()) {
        // 如果是叶子节点，添加键值对到结果中
        result[prefix] = node.as<std::string>();
    } else if (node.IsMap()) {
        // 如果是映射节点，递归解析每个子节点
        for (auto it = node.begin(); it != node.end(); ++it) {
            auto key = it->first.as<std::string>();
            std::string newPrefix;
            if (prefix.empty()) {
                newPrefix = key;
            } else {
                newPrefix = prefix + "/" + key;
            }
            parseYamlNode2(it->second, result, newPrefix);
        }
    } else if (node.IsSequence()) {
        // 如果是序列节点，递归解析每个子节点并添加编号前缀
        int i = 0;
        for (auto it = node.begin(); it != node.end(); ++it) {
            std::string newPrefix = prefix + "[" + std::to_string(i++) + "]";
            parseYamlNode2(*it, result, newPrefix);
        }
    }
}

// 仿造python的处理方式对yaml文件进行处理。
// 从key -> yaml文件

//namespace YAML
//{
//template <>
//struct convert<apollo_client::XmlRpcValue>
//{
//    static Node encode(const apollo_client::XmlRpcValue &rhs)
//    {
//        Node node;
//        return node;
//    }
//
//    static bool decode(const Node &node, apollo_client::XmlRpcValue &rhs)
//    {
//        if (node.IsMap()) {
//            return false;
//        } else if (node.IsScalar()) {
//            rhs = node.as<std::string>();
//            return true;
//        } else if (node.IsSequence()) {
//
//        }
//        return true;
//    }
//};
//} // namespace YAML

void parseYamlNodetest(const YAML::Node &node, std::map<std::string, YAML::Node> &result, const std::string &prefix)
{
    if (node.IsScalar() || node.IsSequence() || node.IsNull()) {
        if (node.IsNull()) {
            result[prefix] = YAML::Node();
        } else {
            result[prefix] = node;
        }
    } else if (node.IsMap()) {
        for (auto it = node.begin(); it != node.end(); ++it) {
            auto key = it->first.as<std::string>();
            std::string newPrefix;
            newPrefix = prefix.empty() ? key : (prefix + "/" + key);
            parseYamlNodetest(it->second, result, newPrefix);
        }
    }
}

void testYaml()
{
    YAML::Node node = YAML::LoadFile(YAML_FILE);
    for (auto it = node.begin(); it != node.end(); ++it) {
        std::cout << it->first.Type() << std::endl;
    }
}

int main()
{
    testYaml();
    return 0;
}