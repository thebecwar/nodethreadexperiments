#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <node.h>
#include "../../libuv/include/UVAsync.h"
#include "JsWorkerThread.h"

#include <queue>

namespace threadpool
{
	class ThreadPool
	{
	public:
		ThreadPool();
		~ThreadPool();

	private:
		

	};
}

#endif 
