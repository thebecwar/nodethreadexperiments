#include "ThreadPool.h"

namespace threadpool
{
    using namespace v8;
    using libuv::threading::UVLock;
    

    // Prototype Methods
    void ThreadPool::Init(v8::Local<v8::Object> exports)
    {
        Isolate* isolate = exports->GetIsolate();

        Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, ThreadPool::New);
        tpl->SetClassName(String::NewFromUtf8(isolate, "ThreadPool"));
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        NODE_SET_PROTOTYPE_METHOD(tpl, "queueItem", ThreadPool::QueueItem);
        NODE_SET_PROTOTYPE_METHOD(tpl, "queuePromise", ThreadPool::QueuePromise);

        tpl->InstanceTemplate()->SetAccessor(
            String::NewFromUtf8(isolate, "queueLength"),
            ThreadPool::GetQueuedItemCount);

        tmplt.Reset(isolate, tpl);
        constructor.Reset(isolate, tpl->GetFunction());
        exports->Set(String::NewFromUtf8(isolate, "ThreadPool"), tpl->GetFunction());
    }
    void ThreadPool::New(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        Isolate* isolate = args.GetIsolate();
        if (!args.IsConstructCall())
        {
            isolate->ThrowException(Exception::SyntaxError(String::NewFromUtf8(isolate, "Must be constructed with new")));
            return;
        }

        std::string initScript("");
        if (args.Length() > 0 && args[0]->IsString())
        {
            Local<String> argval = args[0]->ToString(isolate);
            String::Utf8Value val(argval);
            initScript = *val;
        }

        ThreadPool* self = new ThreadPool(initScript);
        self->Wrap(args.This());
        
        args.GetReturnValue().Set(args.This());

    }
    bool ThreadPool::TypeCheck(v8::Local<v8::Value>& other)
    {
        Isolate* isolate = Isolate::GetCurrent();
        if (isolate == nullptr) return false;
        Local<FunctionTemplate> tpl = tmplt.Get(isolate);
        return tpl->HasInstance(other);
    }

    // JS Methods
    void ThreadPool::QueueItem(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        if (args.Length() == 0)
        {
            args.GetIsolate()->ThrowException(Exception::SyntaxError(String::NewFromUtf8(args.GetIsolate(), "Need a WorkItem")));
            return;
        }

        ThreadPool* self = ObjectWrap::Unwrap<ThreadPool>(args.Holder());
        Local<Value> itemObject = args[0];
        if (JsWorkItem::TypeCheck(itemObject))
        {
            UVLock lock(self->m_workMutex);
            JsWorkItem* item = ObjectWrap::Unwrap<JsWorkItem>(itemObject->ToObject());
            self->m_workQueue.push(item);
        }
        else
        {
            args.GetIsolate()->ThrowException(Exception::SyntaxError(String::NewFromUtf8(args.GetIsolate(), "Need a WorkItem")));
            return;
        }
    }
    void ThreadPool::QueuePromise(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        if (args.Length() == 0)
        {
            args.GetIsolate()->ThrowException(Exception::SyntaxError(String::NewFromUtf8(args.GetIsolate(), "Need a WorkItem")));
            return;
        }

        ThreadPool* self = ObjectWrap::Unwrap<ThreadPool>(args.Holder());
        Local<Value> itemObject = args[0];
        if (JsWorkItem::TypeCheck(itemObject))
        {
            JsWorkItem* item = ObjectWrap::Unwrap<JsWorkItem>(itemObject->ToObject());
            Local<Promise::Resolver> resolver = Promise::Resolver::New(args.GetIsolate());
            item->SetPromise(args.GetIsolate(), resolver);

            args.GetReturnValue().Set(resolver->GetPromise());

            UVLock lock(self->m_workMutex);
            self->m_workQueue.push(item);
        }
        else
        {
            args.GetIsolate()->ThrowException(Exception::SyntaxError(String::NewFromUtf8(args.GetIsolate(), "Need a WorkItem")));
            return;
        }
    }

    // JS Properties
    void ThreadPool::GetQueuedItemCount(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
    {
        ThreadPool* self = ObjectWrap::Unwrap<ThreadPool>(info.Holder());
        //UVLock lock(&self->m_workMutex);
        info.GetReturnValue().Set(Number::New(info.GetIsolate(), (double)self->m_workQueue.size()));
    }
    
    Persistent<Function> ThreadPool::constructor;
    Persistent<FunctionTemplate> ThreadPool::tmplt;

    ThreadPool::ThreadPool(std::string& initScript) : m_initScript(initScript), m_async(this), m_dispatcher(this), m_runDispatcher(true), m_workQueue(), m_completedQueue()
    {
        for (int i = 0; i < MAX_THREADS; i++)
        {
            this->m_workers.emplace_back(new JsWorkerThread(this->m_initScript));
        }
    }

    ThreadPool::~ThreadPool()
    {
        this->m_runDispatcher = false;
        for (auto iter = this->m_workers.begin(); iter != this->m_workers.end(); iter++)
        {
            (*iter)->Stop();
            (*iter)->GetSemaphore().Post();
            (*iter)->GetSemaphore().Post();

            delete *iter;
        }
    }

    void ThreadPool::ThreadProc(libuv::threading::UVThread* thread)
    {
        while (this->m_runDispatcher)
        {
            for (auto iter = this->m_workers.begin(); iter != this->m_workers.end(); iter++)
            {
                if ((*iter)->NeedsWork())
                {
                    UVLock lock(this->m_workMutex);

                    if ((*iter)->GetWorkItem())
                    {
                        UVLock completedLock(this->m_doneMutex);
                        this->m_completedQueue.push((*iter)->GetWorkItem());
						(*iter)->Dispatch(nullptr);
                        this->m_async.Send();
                    }

                    if (!this->m_workQueue.empty())
                    {
                        JsWorkItem* item = this->m_workQueue.front();
                        this->m_workQueue.pop();
                        (*iter)->Dispatch(item);
                        (*iter)->GetSemaphore().Post();
                    }
                }
            }
        }
    }

    void ThreadPool::AsyncCall()
    {
        Isolate* isolate = Isolate::GetCurrent();
		HandleScope handleScope(isolate);

        UVLock lock(this->m_doneMutex);
        while (this->m_completedQueue.size() > 0)
        {
            JsWorkItem* workItem = this->m_completedQueue.front();
            this->m_completedQueue.pop();
            workItem->ExecuteCallback(isolate);
        }
    }
}