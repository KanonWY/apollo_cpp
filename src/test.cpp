#include "apollo_basic.h"
#include <iostream>


int main()
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