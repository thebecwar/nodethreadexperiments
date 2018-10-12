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
    ObjectKey::ObjectKey(v8::Local<v8::Name>& from)
    {
		this->m_isSymbol = from->IsSymbol() || from->IsSymbolObject();
		if (this->m_isSymbol)
		{
			Local<Symbol> sym = Local<Symbol>::Cast(from);
			String::Utf8Value uval(sym->Name()->ToString());
			this->m_value = std::string(*uval);
		}
		else
		{
			String::Utf8Value uval(from->ToString());
			this->m_value = std::string(*uval);
		}
        
    }

    bool ObjectKey::operator==(std::string& other) const
    {
        return (this->m_value == other);
    }
    bool ObjectKey::operator==(ObjectKey& other) const
    {
        return (this->m_value == other.m_value) && (this->m_isSymbol == other.m_isSymbol);
    }
    bool ObjectKey::operator==(v8::Local<v8::Value>& other) const
    {
        ObjectKey otherKey(other);
        if (otherKey.m_value.empty()) return false;
        else
            return this->m_isSymbol == otherKey.m_isSymbol && this->m_value == otherKey.m_value;
    }
    bool ObjectKey::operator<(const ObjectKey other) const
    {
        return this->m_value < other.m_value;
    }

    v8::Local<v8::Value> ObjectKey::Get(v8::Isolate* isolate) const
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
    std::string ObjectKey::Get() const
    {
        return this->m_value;
    }
}