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
        ObjectKey(v8::Local<v8::Name>& from);
        ~ObjectKey() = default;

        bool operator==(std::string& other) const;
        bool operator==(ObjectKey& other) const;
        bool operator==(v8::Local<v8::Value>& other) const;

		bool operator<(const ObjectKey other) const;

        v8::Local<v8::Value> Get(v8::Isolate* isolate) const;
        std::string Get() const;

    private:
        std::string m_value;
        bool m_isSymbol;
    };

}

//namespace std {
//	template<>
//	struct less<sharedobj::ObjectKey>
//	{
//		bool operator()(const ObjectKey left, const ObjectKey right) const
//		{
//			return left.Get() < right.Get();
//		}
//	};
//}
#endif