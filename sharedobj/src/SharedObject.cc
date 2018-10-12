#include "SharedObject.h"

namespace sharedobj
{
    using namespace v8;

    void SharedObject::Init(Local<Object> exports)
    {
        Isolate* isolate = exports->GetIsolate();
        Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, SharedObject::New);
        
        tpl->SetClassName(String::NewFromUtf8(isolate, "SharedObject"));
        tpl->InstanceTemplate()->SetInternalFieldCount(2);

        NamedPropertyHandlerConfiguration namedConfig;
        namedConfig.getter = SharedObject::Get;
        namedConfig.setter = SharedObject::Set;
        namedConfig.query = SharedObject::Query;
        namedConfig.deleter = SharedObject::Deleter;
        namedConfig.enumerator = SharedObject::Enumerator;
        namedConfig.flags = PropertyHandlerFlags::kNone;
        tpl->InstanceTemplate()->SetHandler(namedConfig);

        SharedObject::tmplt.Reset(isolate, tpl);
        SharedObject::constructor.Reset(isolate, tpl->GetFunction());
        exports->Set(String::NewFromUtf8(isolate, "SharedObject"), tpl->GetFunction());
    }
    
    // Internal class. SetWeak on a persistent inside a container doesn't work.
    class DestructWrapper {
    public:
        SharedObject* share;
        Persistent<Value> weakRef;
    };
    void Destruct(const WeakCallbackInfo<DestructWrapper>& data)
    {
        DestructWrapper* wrap = reinterpret_cast<DestructWrapper*>(data.GetParameter());
        SharedObject* obj = wrap->share;

        if (obj)
        {
            int refs = obj->Release();
            if (refs <= 0)
                delete obj;
        }

        wrap->weakRef.Reset();

        delete wrap;
    }
    void SharedObject::New(const FunctionCallbackInfo<Value>& args)
    {
        Isolate* isolate = args.GetIsolate();

        if (!args.IsConstructCall())
        {
            isolate->ThrowException(Exception::SyntaxError(String::NewFromUtf8(isolate, "Must be called with new.")));
            return;
        }

        if (args.Length() == 0 || !args[0]->IsString())
        {
            isolate->ThrowException(Exception::SyntaxError(String::NewFromUtf8(isolate, "Shared object name is required")));
            return;
        }

        String::Utf8Value uval(args[0]->ToString());
        std::string shareName(*uval);

        SharedObject* self;
        if (SharedObject::shares.find(shareName) == SharedObject::shares.end())
        {
            self = new SharedObject();
            SharedObject::shares.emplace(shareName, self);
        }
        else
        {
            self = SharedObject::shares[shareName];
        }
        self->Attach();

        // Store a weak persistent ref to this object so we can track its lifetime
        Local<Object> jsThis = args.This();
        //jsThis->SetAlignedPointerInInternalField(0, self);
        jsThis->SetInternalField(0, External::New(isolate, self));
        //jsThis->SetInternalField(1, External::New(isolate, (void*)refNum)); // Gives us a reference to the persistent for the callback.

        DestructWrapper* wrapper = new DestructWrapper();
        wrapper->share = self;
        wrapper->weakRef.Reset(isolate, jsThis);
        wrapper->weakRef.SetWeak<DestructWrapper>(wrapper, Destruct, WeakCallbackType::kParameter);

        args.GetReturnValue().Set(args.This());

    }
    void SharedObject::NewRef(SharedObject* obj, Isolate* targetIsolate)
    {

    }
    /*void SharedObject::Dtor(const WeakCallbackInfo<SharedObject>& data)
    {
        SharedObject* obj = reinterpret_cast<SharedObject*>(data.GetInternalField(0));
        int refNum = (int)data.GetInternalField(1);

        if (obj)
        {
            int refs = obj->Release();
            if (refs <= 0)
                delete obj;
        }

        
        
        data.GetIsolate()->AdjustAmountOfExternalAllocatedMemory(-10 * 1024 * 1024);
    }*/

    bool SharedObject::TypeCheck(Local<Value>& maybeSharedObject)
    {
        return false;
    }

    // Generic property handlers. (String & Symbol keys)
    void SharedObject::Get(Local<Name> property, const PropertyCallbackInfo<Value>& info)
    {

    }
    void SharedObject::Set(Local<Name> property, Local<Value> value, const PropertyCallbackInfo<Value>& info)
    {

    }
    void SharedObject::Query(Local<Name> property, const PropertyCallbackInfo<Integer>& info)
    {

    }
    void SharedObject::Deleter(Local<Name> property, const PropertyCallbackInfo<Boolean>& info)
    {

    }
    void SharedObject::Enumerator(const PropertyCallbackInfo<Array>& info)
    {

    }

    Persistent<Function> SharedObject::constructor;
    Persistent<FunctionTemplate> SharedObject::tmplt;
    std::map<std::string, SharedObject*> SharedObject::shares;
    //std::map<int, v8::Persistent<v8::Value, v8::CopyablePersistentTraits<v8::Value>>> SharedObject::persistentHandles;
    //std::atomic<int> SharedObject::refCount;

    SharedObject::SharedObject()
    {

    }
    int SharedObject::Attach()
    {
        return this->m_refcnt.fetch_add(1);
    }
    int SharedObject::Release()
    {
        return this->m_refcnt.fetch_sub(1);
    }
}