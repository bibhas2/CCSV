#pragma once

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

#include <stddef.h>
#include <string_view>

namespace ccsv {
    struct FileMapper
    {
#ifdef _WIN32
        //Note: The invalid values for these two handles
        //are weieredly different
        HANDLE file_handle = INVALID_HANDLE_VALUE;
        HANDLE map_handle = NULL;
#else
        int file_handle = -1;
        size_t file_size = 0;
#endif
        std::string_view data;
        bool good();
        FileMapper(const char* file_name);
        virtual ~FileMapper();
        
        std::string_view get_bytes() {
            return data;
        }
    };
}


