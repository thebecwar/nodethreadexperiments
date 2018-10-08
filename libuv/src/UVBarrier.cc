#include "UVBarrier.h"

libuv::threading::UVBarrier::UVBarrier(int count)
{
	int err = uv_barrier_init(&this->m_barrier, count);
}

libuv::threading::UVBarrier::~UVBarrier()
{
	uv_barrier_destroy(&this->m_barrier);
}

int libuv::threading::UVBarrier::Wait()
{
	return uv_barrier_wait(&this->m_barrier);
}
