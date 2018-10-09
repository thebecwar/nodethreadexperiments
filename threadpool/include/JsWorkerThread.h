#ifndef JSWORKERTHREAD_H
#define JSWORKERTHREAD_H

#include <node.h>
#include <node_object_wrap.h>
#include <string>

#include "../libuv/include/UVThread.h"
#include "../libuv/include/UVSemaphore.h"
#include "JsWorkItem.h"

namespace threadpool
{
	// Not intended to be instantiated from Javascript. Only used by the ThreadPool.
	class JsWorkerThread : public libuv::threading::UVThread
	{
	public:
		JsWorkerThread(std::string& initScript);
		~JsWorkerThread();

		void Dispatch(JsWorkItem* workItem);
		
		bool NeedsWork();
		std::string GetSerializedResult();
		JsWorkItem* GetWorkItem();

		void Stop();

		void Callback() override;

		libuv::threading::UVSemaphore& GetSemaphore();

	private:
		std::string m_lastResult;
		std::string m_initScript;
		bool m_continue;
		bool m_waiting;
		libuv::threading::UVSemaphore m_semaphore;
		JsWorkItem* m_current;
		
	};

}

#endif