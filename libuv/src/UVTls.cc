#include "UVTls.h"

libuv::threading::UVTls::Key::Key()
{
	this->m_name = std::string("");
	int err = uv_key_create(&this->m_key);
	if (err < 0)
	{
		// todo: handle error
	}
}

libuv::threading::UVTls::Key::Key(std::string& name) : m_name(name)
{
	int err = uv_key_create(&this->m_key);
	if (err < 0)
	{
		// todo Handle Error
	}
}
libuv::threading::UVTls::Key::Key(const Key& other) : m_key(other.m_key), m_name(other.m_name)
{
}
libuv::threading::UVTls::Key::~Key()
{
	uv_key_delete(&this->m_key);
}
uv_key_t * libuv::threading::UVTls::Key::operator&()
{
	return &this->m_key;
}
std::string libuv::threading::UVTls::Key::GetName()
{
	return std::string(this->m_name);
}

libuv::threading::UVTls::UVTls()
{
}
libuv::threading::UVTls::~UVTls()
{
}

void * libuv::threading::UVTls::Get(std::string & key)
{
	auto item = this->m_keymap.find(key);
	if (item != this->m_keymap.end())
	{
		return uv_key_get(&item->second);
	}
	return nullptr;
}

void libuv::threading::UVTls::Set(std::string & key, void * data)
{
	auto item = this->m_keymap.find(key);
	if (item != this->m_keymap.end())
	{
		this->m_keymap[key] = Key(key);
		Key& k = this->m_keymap[key];
		uv_key_set(&k, data);
	}
	else
	{
		Key& k = item->second;
		uv_key_set(&k, data);
	}
}

void libuv::threading::UVTls::Delete(std::string & key)
{
	auto item = this->m_keymap.find(key);
	if (item != this->m_keymap.end())
	{
		this->m_keymap.erase(key);
	}
}

void libuv::threading::UVTls::GetKeys(std::vector<std::string>& keys)
{
	for (auto iter = this->m_keymap.begin(); iter != this->m_keymap.end(); iter++)
	{
		keys.emplace_back(iter->first);
	}
}


