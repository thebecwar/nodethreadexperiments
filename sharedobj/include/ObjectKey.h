#ifndef OBJECTKEY_H
#define OBJECTKEY_H

#include <node.h>
#include <string>

namespace sharedobj
{
    class ObjectKey
    {
    public:
        ObjectKey(std::string& value, bool isSymbol);
        ObjectKey(v8::Local<v8::Value>& from);
        ~ObjectKey() = default;

        bool operator==(std::string& other);
        bool operator==(ObjectKey& other);
        bool operator==(v8::Local<v8::Value>& other);

        v8::Local<v8::Value> Get(v8::Isolate* isolate);
        std::string Get();

    private:
        std::string m_value;
        bool m_isSymbol;
    };
}

#endif