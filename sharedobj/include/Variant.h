#ifndef VARIANT_H
#define VARIANT_H

#include <node.h>
#include <string>

namespace sharedobj
{
    enum class VariantType
    {
        Undefined = 0,
        Boolean,
        BooleanObject,
        String,
        StringObject,
        Number,
        NumberObject,
    };

	class Variant
	{
    private:
        static void SetValue(Variant* instance, v8::Local<v8::Value>& value);
		
    public:
        Variant();
        Variant(VariantType type);
        Variant(v8::Local<v8::Value>& value);
        Variant(bool value);
        Variant(std::string& value);
        Variant(double value);

        v8::Local<v8::Value> Get(v8::Isolate* isolate);
        void Set(v8::Local<v8::Value>& value);

    private:
        VariantType m_varType;
        bool m_boolValue;
        std::string m_stringValue;
        double m_numberValue;

	};
}

#endif