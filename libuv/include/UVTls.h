#ifndef UVTLS_H
#define UVTLS_H

#include <uv.h>
#include <string>
#include <vector>
#include <map>

namespace libuv
{
namespace threading
{
    class UVTls
    {
    public:
		class Key
		{
		public:
			Key();
			Key(std::string& name);
			~Key();

			uv_key_t* operator&();
			std::string GetName();

		private:
			Key(const Key& other);
			uv_key_t m_key;
			std::string m_name;
		};

    public:
		UVTls();
		~UVTls();

		void* Get(std::string& key);
		void Set(std::string& key, void* data);
		void Delete(std::string& key);

		void GetKeys(std::vector<std::string>& keys);

    private:
		std::map<std::string, Key> m_keymap;
    };
    
}
}


#endif

