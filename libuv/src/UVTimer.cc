#include "UVTimer.h"

namespace libuv
{
namespace callback
{
	UVTimer::UVTimer(IUVTimerCallback* callback) : m_callback(callback), m_timer()
	{
		int err = uv_timer_init(uv_default_loop(), &this->m_timer);
		this->m_timer.data = this;
		if (err < 0)
		{
			//todo handle error
		}
	}

	UVTimer::~UVTimer()
	{
		uv_timer_stop(&this->m_timer);
	}

	void UVTimer::Start(uint64_t timeout, uint64_t repeat)
	{
		uv_timer_start(&this->m_timer, UVTimer::CallbackThunk, timeout, repeat);
	}

	void UVTimer::Stop()
	{
		uv_timer_stop(&this->m_timer);
	}

	void UVTimer::Again()
	{
		uv_timer_again(&this->m_timer);
	}

	void UVTimer::SetRepeat(uint64_t repeat)
	{
		uv_timer_set_repeat(&this->m_timer, repeat);
	}

	uint64_t UVTimer::GetRepeat()
	{
		return uv_timer_get_repeat(&this->m_timer);
	}

	void UVTimer::CallbackThunk(uv_timer_t * handle)
	{
		UVTimer* self = reinterpret_cast<UVTimer*>(handle->data);
		if (self)
			self->m_callback->OnTimer(self);
	}



}
}