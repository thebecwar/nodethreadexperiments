#include "ObjectKey.h"

namespace sharedobj
{
    using namespace v8;

    ObjectKey::ObjectKey(std::string& value, bool isSymbol) : m_value(value), m_isSymbol(isSymbol)
    {
    }
    ObjectKey::ObjectKey(v8::Local<v8::Value>& from)
    {
        if (from->IsName())
        {
            String::Utf8Value uval(from->ToString());
            this->m_value = std::string(*uval);
            this->m_isSymbol = from->IsSymbol() || from->IsSymbolObject();
        }
    }

    bool ObjectKey::operator==(std::string& other)
    {
        return (this->m_value == other);
    }
    bool ObjectKey::operator==(ObjectKey& other)
    {
        return (this->m_value == other.m_value) && (this->m_isSymbol == other.m_isSymbol);
    }
    bool ObjectKey::operator==(v8::Local<v8::Value>& other)
    {
        ObjectKey otherKey(other);
        if (otherKey.m_value.empty()) return false;
        else return this->operator==(otherKey);
    }

    v8::Local<v8::Value> ObjectKey::Get(v8::Isolate* isolate)
    {
        if (this->m_isSymbol)
        {
            return Symbol::New(isolate, String::NewFromUtf8(isolate, this->m_value.c_str()));
        }
        else
        {
            return String::NewFromUtf8(isolate, this->m_value.c_str());
        }
    }
    std::string ObjectKey::Get()
    {
        return this->m_value;
    }
}