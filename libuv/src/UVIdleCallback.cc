#include "UVIdleCallback.h"

namespace libuv
{
namespace callback
{

inline UVIdle::UVIdle(IUVIdleCallback* callback) : m_isActive(false), m_sink(callback), m_idle()
{
	uv_idle_init(uv_default_loop(), &this->m_idle);
	this->m_idle.data = this;
}

inline UVIdle::~UVIdle()
{
	if (this->m_isActive)
		uv_idle_stop(&this->m_idle);
}

void UVIdle::Start()
{
	if (!this->m_isActive)
	{
		this->m_isActive = true;
		uv_idle_start(&this->m_idle, UVIdle::CallbackThunk);
	}
}

void UVIdle::Stop()
{
	if (this->m_isActive)
	{
		this->m_isActive = false;
		uv_idle_stop(&this->m_idle);
	}
}

bool UVIdle::IsActive()
{
	return this->m_isActive;
}

void UVIdle::CallbackThunk(uv_idle_t* handle)
{
	if (handle == nullptr) return;
	UVIdle* self = reinterpret_cast<UVIdle*>(handle->data);
	if (self->m_sink != nullptr)
		self->m_sink->OnIdle();
}

}
}