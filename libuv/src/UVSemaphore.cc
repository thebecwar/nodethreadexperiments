#include "UVSemaphore.h"

libuv::threading::UVSemaphore::UVSemaphore(int value)
{
	int err = uv_sem_init(&this->m_sem, value);
	if (err < 0)
	{
		// todo: handle error
	}
}

libuv::threading::UVSemaphore::~UVSemaphore()
{
	uv_sem_destroy(&this->m_sem);
}

void libuv::threading::UVSemaphore::Post()
{
	uv_sem_post(&this->m_sem);
}

void libuv::threading::UVSemaphore::Wait()
{
	uv_sem_wait(&this->m_sem);
}

int libuv::threading::UVSemaphore::TryWait()
{
	return uv_sem_trywait(&this->m_sem);
}
