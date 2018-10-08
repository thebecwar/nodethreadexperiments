#ifndef UVCONDITION_H
#define UVCONDITION_H

#include <uv.h>

#include "UVMutex.h"

namespace libuv
{
namespace threading
{
    class UVCondition
    {
    public:
		UVCondition();
		~UVCondition();

		void Signal();
		void Broadcast();
		void Wait(UVMutex& mutex);
		int TimedWait(UVMutex& mutex, uint64_t timeout);

    private:
		uv_cond_t m_cond;
    };
    
    
}
}


#endif

