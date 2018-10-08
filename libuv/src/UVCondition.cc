#include "UVCondition.h"

libuv::threading::UVCondition::UVCondition()
{
	uv_cond_init(&this->m_cond);
}

libuv::threading::UVCondition::~UVCondition()
{
	uv_cond_destroy(&this->m_cond);
}

void libuv::threading::UVCondition::Signal()
{
	uv_cond_signal(&this->m_cond);
}

void libuv::threading::UVCondition::Broadcast()
{
	uv_cond_broadcast(&this->m_cond);
}

void libuv::threading::UVCondition::Wait(UVMutex& mutex)
{
	uv_cond_wait(&this->m_cond, &mutex);
}

int libuv::threading::UVCondition::TimedWait(UVMutex& mutex, uint64_t timeout)
{
	return uv_cond_timedwait(&this->m_cond, &mutex, timeout);
}
