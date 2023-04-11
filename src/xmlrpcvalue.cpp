#include <sstream>
#include <mutex>
#include <vector>

#include "xmlrpcvalue.h"

namespace apollo_client
{
XmlRpcValue &XmlRpcValue::operator=(const XmlRpcValue &rhs)
{
    if (this != &rhs) {
        invalidate();
        _type = rhs._type;
        switch (_type) {
        case TypeBoolean:
            _value.asBool = rhs._value.asBool;
            break;
        case TypeInt:
            _value.asInt = rhs._value.asInt;
            break;
        case TypeDouble:
            _value.asDouble = rhs._value.asDouble;
            break;
        case TypeDateTime:
            _value.asTime = new struct tm(*rhs._value.asTime);
            break;
        case TypeString:
            _value.asString = new std::string(*rhs._value.asString);
            break;
        case TypeBase64:
            _value.asBinary = new BinaryData(*rhs._value.asBinary);
            break;
        case TypeArray:
            _value.asArray = new ValueArray(*rhs._value.asArray);
            break;
        case TypeStruct:
            _value.asStruct = new ValueStruct(*rhs._value.asStruct);
            break;
        default:
            _value.asBinary = nullptr;
            break;
        }
    }
    return *this;
}

// Predicate for tm equality
static bool tmEq(struct tm const &t1, struct tm const &t2)
{
    return t1.tm_sec == t2.tm_sec && t1.tm_min == t2.tm_min &&
           t1.tm_hour == t2.tm_hour && t1.tm_mday == t2.tm_mday &&
           t1.tm_mon == t2.tm_mon && t1.tm_year == t2.tm_year;
}

// use recursive?
bool XmlRpcValue::operator==(const XmlRpcValue &other) const
{
    if (_type != other._type)
        return false;

    switch (_type) {
    case TypeBoolean:
        return (!_value.asBool && !other._value.asBool) ||
               (_value.asBool && other._value.asBool);
    case TypeInt:
        return _value.asInt == other._value.asInt;
    case TypeDouble:
        return _value.asDouble == other._value.asDouble;
    case TypeDateTime:
        return tmEq(*_value.asTime, *other._value.asTime);
    case TypeString:
        return *_value.asString == *other._value.asString;
    case TypeBase64:
        return *_value.asBinary == *other._value.asBinary;
    case TypeArray:
        return *_value.asArray == *other._value.asArray;

    // The map<>::operator== requires the definition of value< for kcc
    case TypeStruct: //return *_value.asStruct == *other._value.asStruct;
    {
        if (_value.asStruct->size() != other._value.asStruct->size())
            return false;

        auto it1 = _value.asStruct->begin();
        auto it2 = other._value.asStruct->begin();
        while (it1 != _value.asStruct->end()) {
            const XmlRpcValue &v1 = it1->second;
            const XmlRpcValue &v2 = it2->second;
            if (!(v1 == v2))
                return false;
            it1++;
            it2++;
        }
        return true;
    }
    default:
        break;
    }
    return true; // Both invalid values ...
}

bool XmlRpcValue::operator!=(const XmlRpcValue &other) const
{
    return !(*this == other);
}
int XmlRpcValue::size() const
{
    switch (_type) {
    case TypeString:
        return int(_value.asString->size());
    case TypeBase64:
        return int(_value.asBinary->size());
    case TypeArray:
        return int(_value.asArray->size());
    case TypeStruct:
        return int(_value.asStruct->size());
    default:
        break;
    }
    //    throw XmlRpcException("type error");
    throw std::exception();
}
bool XmlRpcValue::hasMember(const std::string &name) const
{
    return _type == TypeStruct && _value.asStruct->find(name) != _value.asStruct->end();
}

//protected

void XmlRpcValue::invalidate()
{
    switch (_type) {
    case TypeString:
        delete _value.asString;
        break;
    case TypeDateTime:
        delete _value.asTime;
        break;
    case TypeBase64:
        delete _value.asBinary;
        break;
    case TypeArray:
        delete _value.asArray;
        break;
    case TypeStruct:
        delete _value.asStruct;
        break;
    default:
        break;
    }
    _type = TypeInvalid;
    _value.asBinary = nullptr;
}

void XmlRpcValue::assertTypeOrInvalid(Type t) const
{
    if (_type != t)
        throw XmlRpcException("type error");
}

void XmlRpcValue::assertTypeOrInvalid(Type t)
{
    if (_type == TypeInvalid) {
        _type = t;
        switch (_type) { // Ensure there is a valid value for the type
        case TypeString:
            _value.asString = new std::string();
            break;
        case TypeDateTime:
            _value.asTime = new struct tm();
            break;
        case TypeBase64:
            _value.asBinary = new BinaryData();
            break;
        case TypeArray:
            _value.asArray = new ValueArray();
            break;
        case TypeStruct:
            _value.asStruct = new ValueStruct();
            break;
        default:
            _value.asBinary = 0;
            break;
        }
    } else if (_type != t)
        throw XmlRpcException("type error");
}

void XmlRpcValue::assertArray(int size) const
{
    if (_type != TypeArray)
        throw XmlRpcException("type error: expected an array");
    else if (int(_value.asArray->size()) < size)
        throw XmlRpcException("range error: array index too large");
}

void XmlRpcValue::assertArray(int size)
{
    if (_type == TypeInvalid) {
        _type = TypeArray;
        _value.asArray = new ValueArray(size);
    } else if (_type == TypeArray) {
        if (int(_value.asArray->size()) < size)
            _value.asArray->resize(size);
    } else
        throw XmlRpcException("type error: expected an array");
}

void XmlRpcValue::assertStruct() const
{
    if (_type != TypeStruct)
        throw XmlRpcException("type error: expected a struct");
}
void XmlRpcValue::assertStruct()
{
    if (_type == TypeInvalid) {
        _type = TypeStruct;
        _value.asStruct = new ValueStruct();
    } else if (_type != TypeStruct)
        throw XmlRpcException("type error: expected a struct");
}

} // namespace apollo_client
std::ostream &operator<<(std::ostream &os, const apollo_client::XmlRpcValue &v)
{
    // If you want to output in xml format:
    //return os << v.toXml();
    //    return v.write(os);
    
    return os;
}
