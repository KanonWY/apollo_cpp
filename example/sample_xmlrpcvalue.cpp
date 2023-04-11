#include "xmlrpcvalue.h"
#include "param.h"
#include <iostream>
#include <type_traits>
#include <map>
#include <string>
#include <vector>

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

int main()
{
    testSetMap_getMap();
    return 0;
}