#include "UVSharedLib.h"

namespace libuv
{
    
    UVSharedLib::UVSharedLib(const std::string& filename) : m_lib(), m_sharedLibName(filename), m_initialized(false)

    
    {
        int err = uv_dlopen(this->m_sharedLibName.c_str(), &this->m_lib);
        if (err < 0)
        {
            this->m_lastError = std::string(uv_dlerror(nullptr));
        }
        else
        {
            this->m_initialized = true;
        }
    }

    UVSharedLib::~UVSharedLib()
    {
        if (this->m_initialized)
            uv_dlclose(&this->m_lib);
    }

    void* UVSharedLib::FindSymbol(const std::string& name)
    {
        this->m_lastError = "";
        if (!this->m_initialized)
        {
            this->m_lastError = std::string("Library Not Initialized");
            return NULL;
        }

        void* result = NULL;
        int err = uv_dlsym(&this->m_lib, name.c_str(), &result);
        if (err < 0)
        {
            this->m_lastError = std::string(uv_dlerror(&this->m_lib));
            return NULL;
        }
        return result;
    }

    bool UVSharedLib::HasError()
    {
        return !this->m_initialized || !this->m_lastError.empty();
    }

    std::string UVSharedLib::GetLastError()
    {
        return this->m_lastError;
    }

}

