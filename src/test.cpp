#include "apollo_base.h"
#include <iostream>
#include "apollo_client.h"

int testGetPropertiesNobuffer()
{
    //    auto value = ApolloClient::Instance().getValueNoBuffer("CodeBuild");
    //    if(!value.empty())
    //    {
    //        std::cout << "value  = " << value << std::endl;
    //    }

    auto pro = ApolloClient::Instance().getPropertiesNoBuffer();
    DumpProperties(pro);
    return 0;
}

int testCheck()
{
    ApolloClient::NameSpaceNotifyUIDMap m{{"p1_pram_set",-1},{"p2_pram_set",-1}};

    ApolloClient::Instance().testCheckNotify(m);

    return 0;
}


void testBase()
{

}

int main()
{
    apollo_base base;
    auto res = base.getConfigNoBufferInner("http://localhost:8080","cp1","p1_pram_set","default");
    for(const auto& item : res)
    {
        std::cout << item.first << " " << item.second << std::endl;
    }

    return 0;
}