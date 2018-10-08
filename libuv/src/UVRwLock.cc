#include "UVRwLock.h"

libuv::threading::UVRwLock::UVRwLock()
{
	int err = uv_rwlock_init(&this->m_rwlock);
	if (err < 0)
	{
		// todo: Handle error
	}
}

libuv::threading::UVRwLock::~UVRwLock()
{
	uv_rwlock_destroy(&this->m_rwlock);
}

void libuv::threading::UVRwLock::ReadLock()
{
	uv_rwlock_rdlock(&this->m_rwlock);
}
int libuv::threading::UVRwLock::TryReadLock()
{
	return uv_rwlock_tryrdlock(&this->m_rwlock);
}

void libuv::threading::UVRwLock::ReadUnlock()
{
	uv_rwlock_rdunlock(&this->m_rwlock);
}

void libuv::threading::UVRwLock::WriteLock()
{
	uv_rwlock_wrlock(&this->m_rwlock);
}

int libuv::threading::UVRwLock::TryWriteLock()
{
	return uv_rwlock_trywrlock(&this->m_rwlock);
}

void libuv::threading::UVRwLock::WriteUnlock()
{
	uv_rwlock_wrunlock(&this->m_rwlock);
}

libuv::threading::UVReadLock::UVReadLock(UVRwLock& lock) : m_rwlock(lock)
{
	this->m_rwlock.ReadLock();
}

libuv::threading::UVReadLock::~UVReadLock()
{
	this->m_rwlock.ReadUnlock();
}

libuv::threading::UVWriteLock::UVWriteLock(UVRwLock& lock) : m_rwlock(lock)
{
	this->m_rwlock.WriteLock();
}

libuv::threading::UVWriteLock::~UVWriteLock()
{
	this->m_rwlock.WriteUnlock();
}
