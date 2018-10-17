#ifndef UVSHAREDLIB_H
#define UVSHAREDLIB_H

#include <uv.h>
#include <string>
#include <map>

namespace libuv
{
    class UVSharedLib
    {
    public:
        UVSharedLib(const std::string& filename);
        ~UVSharedLib();

        void* FindSymbol(const std::string& name);

        bool HasError();
        std::string GetLastError();

    private:
        uv_lib_t m_lib;
        std::map<std::string, void*> m_foundSymbols;
        std::string m_sharedLibName;
        bool m_initialized;
        std::string m_lastError;

    };

}

#endif