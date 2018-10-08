#include "UVAsync.h"


namespace libuv
{
namespace callback 
{
	UVAsync::UVAsync(IUVAsyncCallback* callback) : m_async(), m_callback(callback)
	{
		uv_async_init(uv_default_loop(), &this->m_async, UVAsync::CallbackThunk);
		this->m_async.data = this;
	}

	UVAsync::~UVAsync()
	{
	}

	void UVAsync::Send()
	{
		uv_async_send(&this->m_async);
	}

	void UVAsync::CallbackThunk(uv_async_t* async)
	{
		UVAsync* self = static_cast<UVAsync*>(async->data);
		self->m_callback->AsyncCall();
	}
}
}

