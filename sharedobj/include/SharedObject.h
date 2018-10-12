#ifndef SHAREDOBJECT_H
#define SHAREDOBJECT_H

#include <node.h>
#include <map>
#include <atomic>
#include <UVRwLock.h>

#include <ObjectKey.h>
#include <Variant.h>

namespace sharedobj
{
	
	class SharedObject
	{
	public:
		static void Init(v8::Local<v8::Object> exports);
		static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void NewRef(SharedObject* obj, v8::Isolate* targetIsolate);

		static bool TypeCheck(v8::Local<v8::Value>& maybeSharedObject);

		// Generic property handlers. (String & Symbol keys)
		static void Get(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
		static void Set(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value>& info);
		static void Query(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Integer>& info);
		static void Deleter(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Boolean>& info);
		static void Enumerator(const v8::PropertyCallbackInfo<v8::Array>& info);


	private:
		static v8::Persistent<v8::Function> constructor;
		static v8::Persistent<v8::FunctionTemplate> tmplt;

        static std::map<std::string, SharedObject*> shares;

	public:
		SharedObject();
		~SharedObject() = default;

        int Attach();
        int Release();

	private:
		std::map<ObjectKey, Variant> m_properties;
		libuv::threading::UVRwLock m_syncLock;
        std::atomic<long> m_refcnt;
	};
}

#endif