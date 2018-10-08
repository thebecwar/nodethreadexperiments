#ifndef UVBARRIER_H
#define UVBARRIER_H

#include <uv.h>

namespace libuv
{
namespace threading
{
	class UVBarrier
	{
	public:
		UVBarrier(int count);
		~UVBarrier();

		int Wait();

	private:
		uv_barrier_t m_barrier;
	};
}
}


#endif

