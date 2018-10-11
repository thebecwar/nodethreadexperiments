#ifndef SHAREDOBJECT_H
#define SHAREDOBJECT_H

#include <node.h>
#include <map>
#include <UVRwLock.h>

#include <Variant.h>

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

	class SharedObject
	{
	public:
		static void Init(v8::Local<v8::Object> exports);
		static void New(v8::FunctionCallbackInfo<v8::Value>& args);
		static void NewRef(SharedObject* obj, v8::Isolate* targetIsolate);
		
		static bool TypeCheck(v8::Local<v8::Value>& maybeSharedObject);

		// Generic property handlers. (String & Symbol keys)
		static void Get(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
		static void Set(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value>& info);
		static void Query(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Integer>& info);
		static void Deleter(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Boolean>& info);
		static void Enumerator(const v8::PropertyCallbackInfo<Array>& info);
		static v8::PropertyHandlerFlags Flags;


	private:
		static v8::Persistent<v8::Function> constructor;
		static v8::Persistent<v8::FunctionTemplate> tmplt;

	public:
		SharedObject();
		~SharedObject() = default;

	private:
		std::map<ObjectKey, Variant> m_properties;
		libuv::threading::UVRwLock syncLock;
	};
}

#endif