#ifndef UVTHREAD_H
#define UVTHREAD_H

#include <uv.h>

#include "UVTls.h"

namespace libuv
{
namespace threading
{
    typedef void (*UVThreadCallback)(void* arg);
    
    class UVThread
    {
    public:
        UVThread(UVThreadCallback threadProc, void* arg);
        ~UVThread();
        
        static UVThread* Self();
        int Join();
        int Equal(UVThread& other);
        
		void* GetThreadData();
		UVThreadCallback GetCallback();
		UVTls& Tls();

    private:
        UVThread() = delete;
        UVThread(const UVThread&) = delete;
        UVThread(const ::uv_thread_t& thread);

		UVTls m_tls;
        uv_thread_t m_handle;
        void* m_arg;
        UVThreadCallback m_callback;

	};
}
}


#endif

