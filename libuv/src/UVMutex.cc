#include "UVMutex.h"

libuv::threading::UVMutex::UVMutex()
{
	int err = uv_mutex_init(&this->m_mutex);
	if (err < 0)
	{
		// todo: handle error
	}
}

libuv::threading::UVMutex::~UVMutex()
{
	uv_mutex_destroy(&this->m_mutex);
}

void libuv::threading::UVMutex::Lock()
{
	uv_mutex_lock(&this->m_mutex);
}

int libuv::threading::UVMutex::TryLock()
{
	return uv_mutex_trylock(&this->m_mutex);
}

void libuv::threading::UVMutex::Unlock()
{
	uv_mutex_unlock(&this->m_mutex);
}

uv_mutex_t* libuv::threading::UVMutex::operator&()
{
	return &this->m_mutex;
}

libuv::threading::UVLock::UVLock(UVMutex& mutex) : m_mutex(mutex)
{
	this->m_mutex.Lock();
}

libuv::threading::UVLock::~UVLock()
{
	this->m_mutex.Unlock();
}
