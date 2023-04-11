#include <sstream>
#include <mutex>
#include <vector>

#include "xmlrpcvalue.h"

namespace apollo_client
{
};

std::ostream &operator<<(std::ostream &os, const apollo_client::XmlRpcValue &v)
{
    // If you want to output in xml format:
    //return os << v.toXml();
    return v.write(os);
}
