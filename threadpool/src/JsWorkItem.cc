#include "JsWorkItem.h"

namespace threadpool
{
    using node::ObjectWrap;
    using namespace v8;

    // Static members
    Persistent<Function> JsWorkItem::constructor;
    Persistent<FunctionTemplate> JsWorkItem::tmplt;

    void JsWorkItem::Init(Local<Object> exports)
    {
        Isolate* isolate = exports->GetIsolate();

        Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
        tpl->SetClassName(String::NewFromUtf8(isolate, "JsWorkItem"));
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        NODE_SET_PROTOTYPE_METHOD(tpl, "runSync", RunSync);
        NODE_SET_PROTOTYPE_METHOD(tpl, "deserializeResult", DeserializeResult);

        tpl->InstanceTemplate()->SetAccessor(
            String::NewFromUtf8(isolate, "script"),
            GetScript);
        tpl->InstanceTemplate()->SetAccessor(
            String::NewFromUtf8(isolate, "serializedResult"),
            GetSerializedResult);

        tmplt.Reset(isolate, tpl);
        constructor.Reset(isolate, tpl->GetFunction());
        exports->Set(String::NewFromUtf8(isolate, "JsWorkItem"), tpl->GetFunction());
    }
    void JsWorkItem::New(const FunctionCallbackInfo<Value>& args)
    {
        Isolate* isolate = args.GetIsolate();
        if (!args.IsConstructCall())
        {
            isolate->ThrowException(Exception::SyntaxError(String::NewFromUtf8(isolate, "Must be constructed with new")));
            return;
        }

        if (args.Length() == 0 || !args[0]->IsString())
        {
            isolate->ThrowException(Exception::SyntaxError(String::NewFromUtf8(isolate, "argument 0: script as string")));
            return;
        }

        String::Utf8Value val(args[0]->ToString(isolate));
        std::string script(*val);

        JsWorkItem* self = new JsWorkItem(script);

        if (args.Length() > 1 && args[1]->IsFunction())
        {
            Local<Function> callback = Local<Function>::Cast(args[1]);
            self->m_callbackFunction.Reset(isolate, callback);

            if (args.Length() > 2)
            {
                for (int i = 0; i < args.Length() - 2; i++)
                {
                    self->m_callbackArgs.emplace_back(Persistent<Value>(isolate, args[i + 2]));
                }
            }

        }

        self->Wrap(args.This());
        args.GetReturnValue().Set(args.This());
    }

    // JS Instance Methods
    void JsWorkItem::RunSync(const FunctionCallbackInfo<Value>& args)
    {
        Local<Context> context = args.GetIsolate()->GetCurrentContext();
        JsWorkItem* self = ObjectWrap::Unwrap<JsWorkItem>(args.Holder());
        Local<Value> result = self->Execute(context);
        args.GetReturnValue().Set(result);
    }
    void JsWorkItem::DeserializeResult(const FunctionCallbackInfo<Value>& args)
    {
        Isolate* isolate = args.GetIsolate();
        JsWorkItem* self = ObjectWrap::Unwrap<JsWorkItem>(args.Holder());
        Local<Value> result = JSON::Parse(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, self->m_result.c_str())).ToLocalChecked();
        args.GetReturnValue().Set(result);
    }


    // JS Properties
    void JsWorkItem::GetScript(Local<String> property, const PropertyCallbackInfo<Value>& info)
    {
        JsWorkItem* self = Unwrap<JsWorkItem>(info.Holder());
        info.GetReturnValue().Set(String::NewFromUtf8(info.GetIsolate(), self->m_script.c_str()));
    }
    void JsWorkItem::GetSerializedResult(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
    {
        JsWorkItem* self = Unwrap<JsWorkItem>(info.Holder());
        info.GetReturnValue().Set(String::NewFromUtf8(info.GetIsolate(), self->m_result.c_str()));
    }

    bool JsWorkItem::TypeCheck(Local<Value>& other)
    {
        Isolate* isolate = Isolate::GetCurrent();
        if (isolate == nullptr)
            return false;
        auto t = tmplt.Get(Isolate::GetCurrent());
        return t->HasInstance(other);
    }

    // Internal Instance Methods
    JsWorkItem::JsWorkItem(std::string& script) : m_script(script)
    {
    }

    Local<Value> JsWorkItem::Execute(Local<Context>& context)
    {
        Isolate* isolate = context->GetIsolate();

        bool ok = this->Compile(context);
        if (!ok)
        {
            // todo: decide what to do with errors
        }

        Local<Script> script = this->m_compiledScripts[isolate].Get(isolate);

        MaybeLocal<Value> maybeResult = script->Run(context);

        if (!maybeResult.IsEmpty())
        {
            Local<Value> result = maybeResult.ToLocalChecked();
            Local<String> serializedResult = JSON::Stringify(context, result->ToObject()).ToLocalChecked();
            String::Utf8Value tmp(serializedResult);
            this->m_result = std::string(*tmp);
            return result;
        }
        else
        {
            return Undefined(context->GetIsolate());
        }
    }

    bool JsWorkItem::Compile(Local<Context>& context)
    {
        Isolate* isolate = context->GetIsolate();

        auto pcs = this->m_compiledScripts.find(isolate);
        if (pcs == this->m_compiledScripts.end())
        {
            Local<String> source = String::NewFromUtf8(isolate, this->m_script.c_str());
            Local<Script> script = Script::Compile(context, source).ToLocalChecked();

            if (this->m_compiledScripts.find(isolate) != this->m_compiledScripts.end())
            {
                this->m_compiledScripts[isolate].Reset(isolate, script);
            }
            else
            {
                this->m_compiledScripts.emplace(isolate, Persistent<Script>(isolate, script));
            }
        }

        return true;
    }

    void JsWorkItem::ExecuteCallback(v8::Isolate* isolate)
    {
        if (!this->m_promiseResolver.IsEmpty())
        {
            HandleScope handleScope(isolate);
            Local<Promise::Resolver> resolver = this->m_promiseResolver.Get(isolate);
            auto maybeLocalResult = JSON::Parse(isolate, String::NewFromUtf8(isolate, this->m_result.c_str()));
            if (maybeLocalResult.IsEmpty())
            {
                resolver->Resolve(Undefined(isolate));
            }
            else
            {
                resolver->Resolve(maybeLocalResult.ToLocalChecked());
            }
        }
        else if (!this->m_callbackFunction.IsEmpty())
        {
            HandleScope handleScope(isolate);

            int argc = (int)this->m_callbackArgs.size();
            if (argc == 0)
            {
                this->m_callbackFunction.Get(isolate)->Call(isolate->GetCurrentContext()->Global(), 0, nullptr);
            }
            else
            {
                std::vector<Local<Value>> localArgs(argc);
                for (int i = 0; i < argc; i++)
                {
                    localArgs[i] = this->m_callbackArgs[i].Get(isolate);
                }
                this->m_callbackFunction.Get(isolate)->Call(isolate->GetCurrentContext()->Global(), argc, &localArgs[0]);
            }
        }
    }

    void JsWorkItem::SetPromise(Isolate* isolate, v8::Local<v8::Promise::Resolver>& resolver)
    {
        this->m_promiseResolver.Reset(isolate, resolver);
    }

    
}