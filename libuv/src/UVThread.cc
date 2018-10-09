#include "UVThread.h"

namespace libuv
{
namespace threading 
{

UVThread::UVThread() : m_handle(), m_callback(nullptr)
{
	int err = uv_thread_create(&this->m_handle, UVThread::CallbackThunk, this);
	if (err < 0)
	{
		// todo: error handling
	}
}
UVThread::UVThread(UVThreadCallback* threadProc) : m_handle(), m_callback(threadProc)
{
    int err = uv_thread_create(&this->m_handle, UVThread::CallbackThunk, this);
    if (err < 0)
    {
        // todo: error handling
    }
}
UVThread::UVThread(const uv_thread_t& thread) : m_handle(), m_callback(nullptr)
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

UVThreadCallback* UVThread::GetCallback()
{
    return this->m_callback;
}
void UVThread::Callback()
{
	if (this->m_callback)
	{
		this->m_callback->ThreadProc(this);
	}
}

void UVThread::CallbackThunk(void* arg)
{
	UVThread* self = reinterpret_cast<UVThread*>(arg);
	self->Callback();
}

UVTls & UVThread::Tls()
{
	return this->m_tls;
}

}
}