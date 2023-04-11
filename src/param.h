#ifndef APOLLO_CPP_CLIENT_PARAM_H
#define APOLLO_CPP_CLIENT_PARAM_H

#include "xmlrpcvalue.h"
#include <vector>
#include <type_traits>
#include <iostream>
#include <spdlog/spdlog.h>

namespace apollo_helper
{
template <typename T>
struct is_vector : std::false_type
{
};

template <typename T, typename Alloc>
struct is_vector<std::vector<T, Alloc>> : std::true_type
{
};
template <typename T>
constexpr bool is_vector_v = is_vector<T>::value;

} // namespace apollo_helper

namespace apollo_client
{
using M_Param = std::map<std::string, XmlRpcValue>;

///////////////////////////////////////////////////////////////////////// [caseable] //////////////////////////////////////////////////////////////////
template <class T>
bool xml_castable(int XmlType)
{
    return false;
}

template <>
bool xml_castable<std::string>(int XmlType)
{
    return XmlType == apollo_client::XmlRpcValue::TypeString;
}

template <>
bool xml_castable<double>(int XmlType)
{
    return (
        XmlType == apollo_client::XmlRpcValue::TypeDouble ||
        XmlType == apollo_client::XmlRpcValue::TypeInt ||
        XmlType == apollo_client::XmlRpcValue::TypeBoolean);
}

template <>
bool xml_castable<float>(int XmlType)
{
    return (
        XmlType == apollo_client::XmlRpcValue::TypeDouble ||
        XmlType == apollo_client::XmlRpcValue::TypeInt ||
        XmlType == apollo_client::XmlRpcValue::TypeBoolean);
}

template <>
bool xml_castable<int>(int XmlType)
{
    return (
        XmlType == apollo_client::XmlRpcValue::TypeDouble ||
        XmlType == apollo_client::XmlRpcValue::TypeInt ||
        XmlType == apollo_client::XmlRpcValue::TypeBoolean);
}

template <>
bool xml_castable<bool>(int XmlType)
{
    return (
        XmlType == apollo_client::XmlRpcValue::TypeDouble ||
        XmlType == apollo_client::XmlRpcValue::TypeInt ||
        XmlType == apollo_client::XmlRpcValue::TypeBoolean);
}
///////////////////////////////////////////////////////////////////////// [cast] ///////////////////////////////////////////////////////////////////////
template <class T>
T xml_cast(apollo_client::XmlRpcValue xml_value)
{
    return static_cast<T>(xml_value);
}

template <>
double xml_cast(apollo_client::XmlRpcValue xml_value)
{
    using namespace apollo_client;
    switch (xml_value.getType()) {
    case XmlRpcValue::TypeDouble:
        return static_cast<double>(xml_value);
    case XmlRpcValue::TypeInt:
        return static_cast<double>(static_cast<int>(xml_value));
    case XmlRpcValue::TypeBoolean:
        return static_cast<double>(static_cast<bool>(xml_value));
    default:
        return 0.0;
    };
}

template <>
float xml_cast(apollo_client::XmlRpcValue xml_value)
{
    using namespace apollo_client;
    switch (xml_value.getType()) {
    case XmlRpcValue::TypeDouble:
        return static_cast<float>(static_cast<double>(xml_value));
    case XmlRpcValue::TypeInt:
        return static_cast<float>(static_cast<int>(xml_value));
    case XmlRpcValue::TypeBoolean:
        return static_cast<float>(static_cast<bool>(xml_value));
    default:
        return 0.0f;
    };
}

template <>
int xml_cast(apollo_client::XmlRpcValue xml_value)
{
    using namespace apollo_client;
    switch (xml_value.getType()) {
    case XmlRpcValue::TypeDouble:
        return static_cast<int>(static_cast<double>(xml_value));
    case XmlRpcValue::TypeInt:
        return static_cast<int>(xml_value);
    case XmlRpcValue::TypeBoolean:
        return static_cast<int>(static_cast<bool>(xml_value));
    default:
        return 0;
    };
}
template <>
bool xml_cast(apollo_client::XmlRpcValue xml_value)
{
    using namespace apollo_client;
    switch (xml_value.getType()) {
    case XmlRpcValue::TypeDouble:
        return static_cast<bool>(static_cast<double>(xml_value));
    case XmlRpcValue::TypeInt:
        return static_cast<bool>(static_cast<int>(xml_value));
    case XmlRpcValue::TypeBoolean:
        return static_cast<bool>(xml_value);
    default:
        return false;
    };
}

/**
 * @brief RosStoreParam Conatiner which is a map< std::string, XmlRpcValue>
 */
class RosStoreContainer
{
public:
    template <class T>
    void setVecImpl(const std::string &key, const std::vector<T> &vec)
    {
        apollo_client::XmlRpcValue xml_vec;
        xml_vec.setSize(vec.size());
        // Copy the contents into the XmlRpcValue
        for (int i = 0; i < vec.size(); i++) {
            xml_vec[i] = vec.at(i);
        }
        g_params[key] = xml_vec;
    }

    template <class T>
    void setMapImpl(const std::string &key, const std::map<std::string, T> &m)
    {
        apollo_client::XmlRpcValue xml_value;
        xml_value.begin();
        for (typename std::map<std::string, T>::const_iterator it = m.begin(); it != m.end(); ++it) {
            xml_value[it->first] = it->second;
        }
        set_(key, xml_value);
    }

    /**
     * @brief set a vector param.
     * @tparam T
     * @param key
     * @param value
     * @return
     */
    template <typename T>
    typename std::enable_if_t<apollo_helper::is_vector_v<T>, void>
    set(const std::string &key, const T &value)
    {
        setVecImpl(key, value);
    }

    /**
     * @brief set a [int/double/float/std::string] param
     * @tparam T
     * @param key
     * @param value
     * @return
     */
    template <typename T>
    typename std::enable_if_t<(std::is_same_v<int, T> || std::is_same_v<double, T> || std::is_same_v<std::string, T>), void>
    set(const std::string &key, const T &value)
    {
        apollo_client::XmlRpcValue v(value);
        set_(key, value);
    }

    /**
     * @brief  set a char array param
     * @tparam T
     * @param key
     * @param value
     * @return
     */
    template <typename T>
    typename std::enable_if_t<std::is_array_v<T>, void>
    set(const std::string &key, const T &value)
    {
        std::string sxx = std::string(value);
        apollo_client::XmlRpcValue v(value);
        set_(key, value);
    }

    /**
     * @brief set a map<std::string, T> param
     * @tparam T
     * @param key
     * @param value
     * @return
     */
    template <typename T>
    typename std::enable_if_t<std::is_same_v<T, std::map<std::string, typename T::mapped_type>>, void>
    set(const std::string &key, const T &value)
    {
        setMapImpl(key, value);
    }

    /**
     * @brief get a vector value
     * @tparam T
     */
    template <typename T>
    typename std::enable_if_t<apollo_helper::is_vector_v<T>, bool>
    get(const std::string &key, T &outValue)
    {
        apollo_client::XmlRpcValue xml_array;
        if (!get_(key, xml_array)) {
            return false;
        }
        if (xml_array.getType() != apollo_client::XmlRpcValue::TypeArray) {
            return false;
        }
        outValue.resize(xml_array.size());
        for (int i = 0; i < xml_array.size(); i++) {
            if (!xml_castable<typename T::value_type>(xml_array[i].getType())) {
                return false;
            }
            outValue[i] = xml_cast<typename T::value_type>(xml_array[i]);
        }
        return true;
    }

    /**
     * @brief get  a [int/double/float/std::string] param
     * @tparam T
     * @param key
     * @param outValue
     * @return
     */
    template <typename T>
    typename std::enable_if_t<(std::is_same_v<int, T> || std::is_same_v<double, T> || std::is_same_v<std::string, T>), bool>
    get(const std::string &key, T &outValue)
    {
        static auto type = apollo_client::XmlRpcValue::Type::TypeInvalid;
        if constexpr (std::is_same_v<int, T>) {
            type = apollo_client::XmlRpcValue::Type::TypeInt;
        } else if constexpr (std::is_same_v<double, T>) {
            type = apollo_client::XmlRpcValue::Type::TypeDouble;
        } else if constexpr (std::is_same_v<std::string, T>) {
            type = apollo_client::XmlRpcValue::Type::TypeString;
        }
        apollo_client::XmlRpcValue value;
        if (!get_(key, value)) {
            return false;
        }
        if (value.getType() != type) {
            return false;
        }
        outValue = T(value);
        return true;
    }

    /**
     * @brief get a char array param
     * @tparam T
     * @param key
     * @param outValue
     * @return
     */
    //    template <typename T>
    //    typename std::enable_if_t<std::is_array_v<T>, bool>
    //    get(const std::string &key, T &outValue)
    //    {
    //        return false;
    //    }

    /**
     * @brief get a map<std::string, T> param
     * @tparam T
     * @param key
     * @param outValue
     * @return
     */
    template <typename T>
    typename std::enable_if_t<std::is_same_v<T, std::map<std::string, typename T::mapped_type>>, bool>
    get(const std::string &key, T &outValue)
    {
        apollo_client::XmlRpcValue xml_value;
        if (!get_(key, xml_value)) {
            return false;
        }
        // Make sure it's a struct type
        if (xml_value.getType() != apollo_client::XmlRpcValue::TypeStruct) {
            return false;
        }
        // Fill the map with stuff
        for (auto &it : xml_value) {
            // Make sure this element is the right type
            if (!xml_castable<typename T::mapped_type>(it.second.getType())) {
                return false;
            }
            // Store the element
            outValue[it.first] = xml_cast<typename T::mapped_type>(it.second);
        }
        return true;
    }

    void dump()
    {
        std::cout << "g_params size = " << g_params.size() << std::endl;
    }

private:
    /**
     * @brief inner function to get!
     * @param key
     * @param value
     */
    void set_(const std::string &key, const apollo_client::XmlRpcValue &value)
    {
        g_params[key] = value;
    }

    bool get_(const std::string &key, apollo_client::XmlRpcValue &value)
    {
        auto res = g_params.find(key);
        if (res != g_params.end()) {
            value = res->second;
            return true;
        } else {
            return false;
        }
    }

private:
    M_Param g_params;
    std::mutex g_params_mutex;
};

} // namespace apollo_client

#endif //APOLLO_CPP_CLIENT_PARAM_H
