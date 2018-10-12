#include "SharedObject.h"

namespace sharedobj
{
    using namespace v8;
	using libuv::threading::UVReadLock;
	using libuv::threading::UVWriteLock;

    void SharedObject::Init(Local<Object> exports)
    {
        Isolate* isolate = exports->GetIsolate();
        Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, SharedObject::New);
        
        tpl->SetClassName(String::NewFromUtf8(isolate, "SharedObject"));
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        NamedPropertyHandlerConfiguration namedConfig;
        namedConfig.getter = SharedObject::Get;
        namedConfig.setter = SharedObject::Set;
        namedConfig.query = SharedObject::Query;
        namedConfig.deleter = SharedObject::Deleter;
        namedConfig.enumerator = SharedObject::Enumerator;
        namedConfig.flags = PropertyHandlerFlags::kNone;
        tpl->InstanceTemplate()->SetHandler(namedConfig);

		NODE_SET_PROTOTYPE_METHOD(tpl, "tryLockRead", TryLockRead);
		NODE_SET_PROTOTYPE_METHOD(tpl, "lockRead", LockRead);
		NODE_SET_PROTOTYPE_METHOD(tpl, "unlockRead", UnlockRead);
		NODE_SET_PROTOTYPE_METHOD(tpl, "tryLockWrite", TryLockWrite);
		NODE_SET_PROTOTYPE_METHOD(tpl, "lockWrite", LockWrite);
		NODE_SET_PROTOTYPE_METHOD(tpl, "unlockWrite", UnlockWrite);

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
        jsThis->SetAlignedPointerInInternalField(0, self);
        
        DestructWrapper* wrapper = new DestructWrapper();
        wrapper->share = self;
        wrapper->weakRef.Reset(isolate, jsThis);
        wrapper->weakRef.SetWeak<DestructWrapper>(wrapper, Destruct, WeakCallbackType::kParameter);

        args.GetReturnValue().Set(args.This());

    }

    bool SharedObject::TypeCheck(Local<Value>& maybeSharedObject)
    {
        return false;
    }

    // Generic property handlers. (String & Symbol keys)
    void SharedObject::Get(Local<Name> property, const PropertyCallbackInfo<Value>& info)
    {
		Isolate* isolate = info.GetIsolate();
		SharedObject* self = reinterpret_cast<SharedObject*>(info.Holder()->GetAlignedPointerFromInternalField(0));

		UVReadLock readLock(self->m_syncLock);

		ObjectKey key(property);
		if (self->m_properties.find(key) != self->m_properties.end())
		{
			info.GetReturnValue().Set(self->m_properties[key].Get(isolate));
		}
		else
		{
			info.GetReturnValue().Set(Undefined(isolate));
		}
    }
    void SharedObject::Set(Local<Name> property, Local<Value> value, const PropertyCallbackInfo<Value>& info)
    {
		Isolate* isolate = info.GetIsolate();
		SharedObject* self = reinterpret_cast<SharedObject*>(info.Holder()->GetAlignedPointerFromInternalField(0));

		UVWriteLock writeLock(self->m_syncLock);

		ObjectKey key(property);
		if (self->m_properties.find(key) == self->m_properties.end())
		{
			self->m_properties.emplace(key, Variant(value));
		}
		else
		{
			self->m_properties[key] = Variant(value);
		}
    }
    void SharedObject::Query(Local<Name> property, const PropertyCallbackInfo<Integer>& info)
    {
		info.GetReturnValue().Set(0);
    }
    void SharedObject::Deleter(Local<Name> property, const PropertyCallbackInfo<Boolean>& info)
    {
		Isolate* isolate = info.GetIsolate();
		SharedObject* self = reinterpret_cast<SharedObject*>(info.Holder()->GetAlignedPointerFromInternalField(0));

		ObjectKey key(property);
		if (self->m_properties.find(key) != self->m_properties.end())
		{
			UVWriteLock writeLock(self->m_syncLock);
			self->m_properties.erase(key);
			info.GetReturnValue().Set(true);
		}
		else
		{
			info.GetReturnValue().Set(false);
		}
    }
    void SharedObject::Enumerator(const PropertyCallbackInfo<Array>& info)
    {
		Isolate* isolate = info.GetIsolate();
		SharedObject* self = reinterpret_cast<SharedObject*>(info.Holder()->GetAlignedPointerFromInternalField(0));

		size_t count = self->m_properties.size();
		Local<Array> result = Array::New(isolate, (int)count);
		int i = 0;
		for (auto iter = self->m_properties.begin(); iter != self->m_properties.end(); iter++)
		{
			result->Set(i++, iter->first.Get(isolate));
		}
		info.GetReturnValue().Set(result);
    }

	// Shared Synchronization
	void SharedObject::TryLockRead(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
		SharedObject* self = reinterpret_cast<SharedObject*>(args.Holder()->GetAlignedPointerFromInternalField(0));
		args.GetReturnValue().Set(self->TryLockRead());
    }
	void SharedObject::LockRead(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
		SharedObject* self = reinterpret_cast<SharedObject*>(args.Holder()->GetAlignedPointerFromInternalField(0));
		self->LockRead();
    }
	void SharedObject::UnlockRead(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
		SharedObject* self = reinterpret_cast<SharedObject*>(args.Holder()->GetAlignedPointerFromInternalField(0));
		self->UnlockRead();
    }
		 
	void SharedObject::TryLockWrite(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
		SharedObject* self = reinterpret_cast<SharedObject*>(args.Holder()->GetAlignedPointerFromInternalField(0));
		args.GetReturnValue().Set(self->TryLockWrite());
    }
	void SharedObject::LockWrite(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
		SharedObject* self = reinterpret_cast<SharedObject*>(args.Holder()->GetAlignedPointerFromInternalField(0));
		self->LockWrite();
    }
	void SharedObject::UnlockWrite(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
		SharedObject* self = reinterpret_cast<SharedObject*>(args.Holder()->GetAlignedPointerFromInternalField(0));
		self->UnlockWrite();
    }

    Persistent<Function> SharedObject::constructor;
    Persistent<FunctionTemplate> SharedObject::tmplt;
    std::map<std::string, SharedObject*> SharedObject::shares;

	SharedObject::SharedObject() : m_refcnt(0)
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

	bool SharedObject::TryLockRead()
	{
		return this->m_syncLock.TryReadLock() == 0;
	}
	void SharedObject::LockRead()
	{
		this->m_syncLock.ReadLock();
	}
	void SharedObject::UnlockRead()
	{
		this->m_syncLock.ReadUnlock();
	}

	bool SharedObject::TryLockWrite()
	{
		return this->m_syncLock.TryWriteLock() == 0;
	}
	void SharedObject::LockWrite()
	{
		this->m_syncLock.WriteLock();
	}
	void SharedObject::UnlockWrite()
	{
		this->m_syncLock.ReadUnlock();
	}
}