#ifndef JSWORKITEM_H
#define JSWORKITEM_H

#include <node.h>
#include <node_object_wrap.h>
#include <string>
#include <map>

namespace threadpool
{
    class JsWorkItem : public node::ObjectWrap
    {
    // JS Object Support
    public:
        // Prototype Methods
        static void Init(v8::Local<v8::Object> exports);
        static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
        static bool TypeCheck(v8::Local<v8::Value>& other);

        // JS Instance Methods
        static void RunSync(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void DeserializeResult(const v8::FunctionCallbackInfo<v8::Value>& args);

        // JS Properties
        static void GetScript(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
        static void GetSerializedResult(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
        

    private:
        static v8::Persistent<v8::Function> constructor;
        static v8::Persistent<v8::FunctionTemplate> JsWorkItem::tmplt;

    // Internal Methods
    public:
        JsWorkItem(std::string& script);
        ~JsWorkItem() = default;

        v8::Local<v8::Value> Execute(v8::Local<v8::Context>& context);
        void ExecuteCallback(v8::Isolate* isolate);

        void SetPromise(v8::Isolate* isolate, v8::Local<v8::Promise::Resolver>& resolver);

    private:
        bool Compile(v8::Local<v8::Context>& context);

        std::string m_script;
        std::string m_result;
        std::map<v8::Isolate*, v8::Persistent<v8::Script, v8::CopyablePersistentTraits<v8::Script>>> m_compiledScripts;
        v8::Persistent<v8::Function> m_callbackFunction;
        v8::Persistent<v8::Promise::Resolver> m_promiseResolver;
        std::vector<v8::Persistent<v8::Value, v8::CopyablePersistentTraits<v8::Value>>> m_callbackArgs;

    };
}


#endif