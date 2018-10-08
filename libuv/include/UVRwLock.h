#ifndef UVRWLOCK_H
#define UVRWLOCK_H

#include <uv.h>

namespace libuv
{
namespace threading
{
	class UVRwLock
	{
	public:
		UVRwLock();
		~UVRwLock();

		void ReadLock();
		int TryReadLock();
		void ReadUnlock();

		void WriteLock();
		int TryWriteLock();
		void WriteUnlock();

	private:
		uv_rwlock_t m_rwlock;
	};

	class UVReadLock
	{
	public:
		UVReadLock(UVRwLock& lock);
		~UVReadLock();

	private:
		UVRwLock& m_rwlock;
	};

	class UVWriteLock
	{
	public:
		UVWriteLock(UVRwLock& lock);
		~UVWriteLock();

	private:
		UVRwLock& m_rwlock;
	};
}
}


#endif

