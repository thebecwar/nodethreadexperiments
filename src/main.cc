
#include <node.h>

#include <JsWorkItem.h>
#include <ThreadPool.h>
#include <SharedObject.h>

using v8::Local;
using v8::Object;

void InitAll(Local<Object> exports)
{
    threadpool::JsWorkItem::Init(exports);
    threadpool::ThreadPool::Init(exports);

    sharedobj::SharedObject::Init(exports);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, InitAll);