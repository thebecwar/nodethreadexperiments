
#include <node.h>

#include <JsWorkItem.h>
#include <ThreadPool.h>

using v8::Local;
using v8::Object;

void InitAll(Local<Object> exports)
{
	threadpool::JsWorkItem::Init(exports);
	threadpool::ThreadPool::Init(exports);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, InitAll);