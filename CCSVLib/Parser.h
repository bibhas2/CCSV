#pragma once

#include <string_view>
#include <functional>
#include <span>

/**
 A non-allocating CSV parser that follows https://www.rfc-editor.org/rfc/rfc4180
 */
namespace ccsv {
struct Reader
{
    size_t start = 0;
    size_t end = 0;
    size_t position = 0;
    
    virtual char peek() = 0;
    virtual char pop() = 0;
    virtual void putback() = 0;
    virtual bool good() = 0;
    virtual void mark_start() = 0;
    virtual void mark_end() = 0;
    virtual std::string_view segment() = 0;
    virtual ~Reader() {};
};

void _parse(Reader& reader, std::span<std::string_view> storage, std::function<void(size_t, std::span<std::string_view>)> consumer);

template <size_t MaxFields>
void parse(Reader& reader, std::function<void(size_t, std::span<std::string_view>)> consumer) {
    //Statically allocate memory for the fields of a record (line in CSV).
    std::string_view array[MaxFields];
    std::span<std::string_view> storage{array, MaxFields};
    
    _parse(reader, storage, consumer);
}

}
