#ifndef UVASYNC_H
#define UVASYNC_H

#include <uv.h>

namespace libuv
{
namespace callback
{
	class IUVAsyncCallback
	{
	public:
		virtual void AsyncCall() = 0;
	};

	class UVAsync
	{
	public:
		UVAsync(IUVAsyncCallback* callback);
		~UVAsync();

		void Send();

	private:
		uv_async_t m_async;
		IUVAsyncCallback* m_callback;

		static void CallbackThunk(uv_async_t* async);

	};
}
}




#endif
