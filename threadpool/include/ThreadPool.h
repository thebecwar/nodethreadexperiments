#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <node.h>

#include <string>
#include <queue>
#include <vector>

#include <UVAsync.h>
#include <UVThread.h>
#include <UVMutex.h>
#include <JsWorkerThread.h>

namespace threadpool
{
	class ThreadPool : public libuv::callback::IUVAsyncCallback, public libuv::threading::UVThreadCallback, public node::ObjectWrap
	{
	public:
		// Prototype Methods
		static void Init(v8::Local<v8::Object> exports);
		static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
		static bool TypeCheck(v8::Local<v8::Value>& other);

		// JS Methods
		static void QueueItem(const v8::FunctionCallbackInfo<v8::Value>& args);

		// JS Properties
		static void GetQueuedItemCount(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);

	private:
		static v8::Persistent<v8::Function> constructor;
		static v8::Persistent<v8::FunctionTemplate> tmplt;

		const int MAX_THREADS = 1;

	public:
		ThreadPool(std::string& initScript);
		~ThreadPool();

		void ThreadProc(libuv::threading::UVThread* thread) override;
		void AsyncCall() override;

	private:
		std::string m_initScript;
		std::vector<JsWorkerThread*> m_workers;
		std::queue<JsWorkItem*> m_workQueue;
		std::queue<JsWorkItem*> m_completedQueue;
		libuv::callback::UVAsync m_async;
		libuv::threading::UVThread m_dispatcher;
		libuv::threading::UVMutex m_workMutex;
        libuv::threading::UVMutex m_doneMutex;
		bool m_runDispatcher;
	};
}

#endif 
