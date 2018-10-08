#ifndef UVTIMER_H
#define UVTIMER_H

#include <cstdint>
#include <uv.h>

namespace libuv
{
namespace callback
{
	class IUVTimerCallback;

	class UVTimer
	{
	public:
		UVTimer(IUVTimerCallback* callback);
		~UVTimer();

		void Start(uint64_t timeout, uint64_t repeat);
		void Stop();
		void Again();
		void SetRepeat(uint64_t repeat);
		uint64_t GetRepeat();

	private:
		static void CallbackThunk(uv_timer_t* handle);
		IUVTimerCallback* m_callback;
		uv_timer_t m_timer;
	};

	class IUVTimerCallback
	{
	public:
		virtual void OnTimer(UVTimer* timerInstance) = 0;
	};
}
}


#endif