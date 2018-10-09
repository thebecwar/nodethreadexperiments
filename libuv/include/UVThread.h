#ifndef UVTHREAD_H
#define UVTHREAD_H

#include <uv.h>

#include "UVTls.h"

namespace libuv
{
namespace threading
{
	class UVThreadCallback;
	
    class UVThread
    {
    public:
		UVThread();
        UVThread(UVThreadCallback* threadProc);
        ~UVThread();
        
        static UVThread* Self();
        int Join();
        int Equal(UVThread& other);
        
		void* GetThreadData();
		UVThreadCallback* GetCallback();
		UVTls& Tls();

    protected:
		virtual void Callback();

    private:
        UVThread(const UVThread&) = delete;
        UVThread(const ::uv_thread_t& thread);

		static void CallbackThunk(void* arg);

		UVTls m_tls;
        uv_thread_t m_handle;
        UVThreadCallback* m_callback;

	};

	class UVThreadCallback
	{
	public:
		virtual void ThreadProc(UVThread* thread) = 0;
	};
}
}


#endif

