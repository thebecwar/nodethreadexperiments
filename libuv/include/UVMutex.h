#ifndef UVMUTEX_H
#define UVMUTEX_H

#include <uv.h>

namespace libuv
{
namespace threading
{
    class UVMutex
    {
    public:
        UVMutex();
        ~UVMutex();

        void Lock();
        int TryLock();
        void Unlock();

    	uv_mutex_t* operator&();

    private:
        uv_mutex_t m_mutex;
    };

    class UVLock
    {
    public:
        UVLock(UVMutex& mutex);
        ~UVLock();

    private:
        UVMutex &m_mutex;
    };
    
    
}
}


#endif

