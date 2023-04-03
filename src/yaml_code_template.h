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
} // namespace YAML

#endif