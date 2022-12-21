#include "MemoryMappedReader.h"

#ifndef _WIN32
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#endif

namespace ccsv {
    MemoryMappedReader::MemoryMappedReader(const char* file_name) : StringReader() {
#ifdef _WIN32
        file_handle = ::CreateFileA(file_name, GENERIC_READ, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

        if (file_handle == INVALID_HANDLE_VALUE)
        {
            return;
        }

        map_handle = ::CreateFileMappingA(file_handle, NULL, PAGE_READONLY, 0, 0, NULL);

        if (map_handle == INVALID_HANDLE_VALUE)
        {
            return;
        }

        const char* buff = (const char*) ::MapViewOfFile(map_handle, FILE_MAP_READ, 0, 0, 0);

        if (buff == NULL) {
            return;
        }

        DWORD file_size = ::GetFileSize(file_handle, NULL);

        data = std::string_view(buff, file_size);
#endif
#ifndef _WIN32
        file_handle = ::open(file_name, O_RDONLY);
        
        if (file_handle < 0) {
            perror("open() failed");
            
            return;
        }
        
        struct stat sbuf;
        
        if (stat(file_name, &sbuf) == -1) {
            perror("stat() failed");
            
            return;
        }

        file_size = sbuf.st_size;
        
        void *start = ::mmap(nullptr, file_size, PROT_READ, MAP_FILE | MAP_SHARED, file_handle, 0);
        
        if (start == MAP_FAILED) {
            perror("mmap() failed");
            
            return;
        }
        
        data = std::string_view(reinterpret_cast<const char*>(start), file_size);
#endif
    }

    MemoryMappedReader::~MemoryMappedReader() {
#ifdef _WIN32
        if (map_handle != INVALID_HANDLE_VALUE) {
            if (data.data() != nullptr) {
                ::UnmapViewOfFile(data.data());
            }

            ::CloseHandle(map_handle);

            map_handle = INVALID_HANDLE_VALUE;
        }
        if (file_handle != INVALID_HANDLE_VALUE) {
            ::CloseHandle(file_handle);

            file_handle = INVALID_HANDLE_VALUE;
        }
#endif
#ifndef _WIN32
        if (::munmap((void*) data.data(), file_size) < 0) {
            perror("munmap() failed.");
        }
        
        if (::close(file_handle) < 0) {
            perror("close() failed.");
        }
#endif
    }

bool MemoryMappedReader::good() {
#ifdef _WIN32
        return (file_handle != INVALID_HANDLE_VALUE) && (map_handle != INVALID_HANDLE_VALUE);
#endif
#ifndef _WIN32
        return (file_handle >= 0) && (data.data() != nullptr);
#endif
}
}
