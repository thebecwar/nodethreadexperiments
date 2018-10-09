#include "JsWorkerThread.h"

namespace threadpool
{
	using namespace v8;
	using libuv::threading::UVSemaphore;
	using libuv::threading::UVThread;

	JsWorkerThread::JsWorkerThread(std::string& initScript) : m_initScript(initScript),
	                                                          m_continue(true),
															  m_semaphore(0),
															  m_current(nullptr)
	{
	}

	JsWorkerThread::~JsWorkerThread()
	{
	}

	void JsWorkerThread::Dispatch(JsWorkItem* workItem)
	{
		this->m_current = workItem;
	}
	bool JsWorkerThread::NeedsWork()
	{
		return this->m_current == nullptr && this->m_waiting;
	}
	std::string JsWorkerThread::GetSerializedResult()
	{
		return this->m_lastResult;
	}

	JsWorkItem* JsWorkerThread::GetWorkItem()
	{
		return this->m_current;
	}

	void JsWorkerThread::Stop()
	{
		this->m_continue = false;
	}

	UVSemaphore& JsWorkerThread::GetSemaphore()
	{
		return this->m_semaphore;
	}

	void JsWorkerThread::Callback()
	{
		// Initialize JS Environment
		Isolate::CreateParams params;
		params.array_buffer_allocator = ArrayBuffer::Allocator::NewDefaultAllocator();
		Isolate* isolate = Isolate::New(params);
		{
			Isolate::Scope isolateScope(isolate);
			Locker locker(isolate);
			HandleScope handleScope(isolate);
			Local<ObjectTemplate> globals = ObjectTemplate::New(isolate);
			// TODO: initialize globals
			Local<Context> context = Context::New(isolate, NULL, globals);
			Context::Scope contextScope(context);

			// Execute Init Script
			{
				JsWorkItem item = JsWorkItem(this->m_initScript);
				item.Execute(context);
			}

			while (this->m_continue)
			{
				this->m_waiting = true;
				this->m_semaphore.Wait();
				this->m_waiting = false;

				if (this->m_current)
				{
					this->m_current->Execute(context);
				}
			}

		} // Isolate scope
		isolate->Dispose();
		delete params.array_buffer_allocator;
	}

	
}