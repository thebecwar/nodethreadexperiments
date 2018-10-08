#ifndef UVSEMAPHORE_H
#define UVSEMAPHORE_H

#include <uv.h>

namespace libuv
{
namespace threading
{
    class UVSemaphore
    {
    public:
		UVSemaphore(int value);
		~UVSemaphore();

		void Post();
		void Wait();
		int TryWait();

    private:
		uv_sem_t m_sem;
    };

}
}

#endif

