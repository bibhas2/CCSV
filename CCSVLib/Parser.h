#pragma once

#include <string_view>
#include <functional>
#include <span>
#include <charconv>

/**
 A non-allocating CSV parser that follows https://www.rfc-editor.org/rfc/rfc4180
 */
namespace ccsv {
enum ParseStatus {
    HAS_MORE_FIELDS,
    END_RECORD,
    END_DOCUMENT
};

struct Parser
{
    size_t start = 0;
    size_t stop = 0;
    size_t position = 0;
    
    char peek(const std::string_view& data);
    char pop(const std::string_view& data);
    void putback();
    
    /*
     * The next read character will mark the beginning of a field.
     */
    void mark_start();
    /*
     * The last read character stops a field, such as a comma or CR.
     * Hence, this character is excluded from the field.
     */
    void mark_stop();
    /*
     * Return the characters between the
     * start (inclusive) and stop (exclusive).
     */
    std::string_view segment(const std::string_view& data);
    ~Parser() {};
    ParseStatus next_field(const std::string_view& data, std::string_view& field);
    ParseStatus parse_record(const std::string_view& data, std::span<std::string_view> storage, std::span<std::string_view>& record);
    
    template <size_t MaxFields>
    void parse(std::string_view data, std::function<void(size_t, std::span<std::string_view>)> consumer) {
        //Statically allocate memory for the fields of a record (line in CSV).
        std::string_view array[MaxFields];
        std::span<std::string_view> storage{array};
        std::span<std::string_view> record;
        size_t index = 0;
        
        while (parse_record(data, storage, record) != END_DOCUMENT) {
            consumer(index, record);
            
            ++index;
        }
    }
};

std::string_view trim(const std::string_view& str);

#ifndef __clang__
template <typename T>
bool parse_number(std::string_view bytes, T& n) {
    auto tr = ccsv::trim(bytes);

    auto result = std::from_chars(tr.data(), tr.data() + tr.length(), n);

    return result.ec == std::errc();
}
#endif
}
