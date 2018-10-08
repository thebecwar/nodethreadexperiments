#include "UVThread.h"

namespace libuv
{
namespace threading 
{
UVThread::UVThread(UVThreadCallback threadProc, void* arg) : m_handle(), m_arg(arg), m_callback(threadProc)
{
    int err = uv_thread_create(&this->m_handle, m_callback, m_arg);
    if (err < 0)
    {
        // todo: error handling
    }
}
UVThread::UVThread(const uv_thread_t& thread) : m_handle(), m_arg(NULL), m_callback(nullptr)
{
    memcpy(&this->m_handle, &thread, sizeof(uv_thread_t));
}
UVThread::~UVThread()
{
}

UVThread* UVThread::Self()
{
    return new UVThread(uv_thread_self());
}
int UVThread::Join()
{
    return uv_thread_join(&this->m_handle);
}
int UVThread::Equal(UVThread& other)
{
    return uv_thread_equal(&this->m_handle, &(other.m_handle));
}

void* UVThread::GetThreadData()
{
    return this->m_arg;
}
UVThreadCallback UVThread::GetCallback()
{
    return this->m_callback;
}

UVTls & UVThread::Tls()
{
	return this->m_tls;
}

}
}