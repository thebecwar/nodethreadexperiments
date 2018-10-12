#include "Variant.h"

namespace sharedobj
{
    void Variant::SetValue(Variant* instance, v8::Local<v8::Value>& value)
    {
        instance->m_varType = VariantType::Undefined;
        instance->m_boolValue = false;
        instance->m_numberValue = NAN;
        instance->m_stringValue = std::string("");

        if (value->IsBoolean())
        {
            instance->m_varType = VariantType::Boolean;
            instance->m_boolValue = value->BooleanValue();
        }
        else if (value->IsBooleanObject())
        {
            instance->m_varType = VariantType::BooleanObject;
            instance->m_boolValue = value->BooleanValue();
        }
        else if (value->IsNumber())
        {
            instance->m_varType = VariantType::Number;
            instance->m_numberValue = value->NumberValue();
        }
        else if (value->IsNumberObject())
        {
            instance->m_varType = VariantType::NumberObject;
            instance->m_numberValue = value->NumberValue();
        }
        else if (value->IsString())
        {
            instance->m_varType = VariantType::String;
            v8::String::Utf8Value val(value->ToString());
            instance->m_stringValue = std::string(*val);
        }
        else if (value->IsStringObject())
        {
            instance->m_varType = VariantType::StringObject;
            v8::String::Utf8Value val(value->ToString());
            instance->m_stringValue = std::string(*val);
        }
    }

    Variant::Variant() : m_varType(VariantType::Undefined), m_numberValue(NAN), m_boolValue(false), m_stringValue()
    {
    }
    Variant::Variant(VariantType type) : m_varType(type)
    {
    }
    Variant::Variant(v8::Local<v8::Value>& value)
    {
        Variant::SetValue(this, value);
    }
    Variant::Variant(bool value) : m_varType(VariantType::Boolean), m_boolValue(value)
    {
    }
    Variant::Variant(std::string& value) : m_varType(VariantType::String), m_stringValue(value)
    {
    }
    Variant::Variant(double value) : m_varType(VariantType::Number), m_numberValue(value)
    {
    }

    v8::Local<v8::Value> Variant::Get(v8::Isolate* isolate)
    {
        switch (this->m_varType)
        {
        case VariantType::Boolean:
            return v8::Boolean::New(isolate, this->m_boolValue);
        case VariantType::BooleanObject:
            return v8::BooleanObject::New(isolate, this->m_boolValue);
        case VariantType::Number:
            return v8::Number::New(isolate, this->m_numberValue);
        case VariantType::NumberObject:
            return v8::NumberObject::New(isolate, this->m_numberValue);
        case VariantType::String:
            return v8::String::NewFromUtf8(isolate, this->m_stringValue.c_str());
        case VariantType::StringObject:
            return v8::StringObject::New(v8::String::NewFromUtf8(isolate, this->m_stringValue.c_str()));
        case VariantType::Undefined:
        default:
            return v8::Undefined(isolate);
        }
    }
    void Variant::Set(v8::Local<v8::Value>& value)
    {
        Variant::SetValue(this, value);
    }
}