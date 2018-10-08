#ifndef UVIdle_H
#define UVIdle_H

#include <uv.h>

namespace libuv
{
namespace callback
{
	class IUVIdleCallback
	{
	public:
		virtual void OnIdle() = 0;
	};


	class UVIdle
	{
	public:
		UVIdle(IUVIdleCallback* callback);
		~UVIdle();

		void Start();
		void Stop();
		bool IsActive();

	private:
		static void CallbackThunk(uv_idle_t* handle);
		bool m_isActive;
		IUVIdleCallback* m_sink;
		uv_idle_t m_idle;

	};
}
}


#endif

