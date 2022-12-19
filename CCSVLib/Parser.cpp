#include "Parser.h"

namespace ccsv {
enum ParseStatus {
    HAS_MORE_FIELDS,
    END_RECORD,
    END_DOCUMENT
};

void eat_space(Reader& reader) {
    char ch = reader.pop();

    if (ch == 0) return;

    while (isspace(ch)) {
        ch = reader.pop();

        if (ch == 0) return;
    }

    reader.putback();
}

std::string_view extract_segment(Reader& reader, bool escaped_field) {
    if (!escaped_field) {
        return reader.segment();
    }
    
    std::string_view field = reader.segment();
    
    return field.substr(1, field.length() - 2);
}

ParseStatus next_field(Reader& reader, std::string_view& field) {
    bool inside_dquote = false;
    bool escaped_field = false;
    
    reader.mark_start();
    
    while (true) {
        char ch = reader.pop();
        
        if (ch == '\0') {
            reader.mark_end();
            field = reader.segment();
            
            return END_DOCUMENT;
        }
        
        if (ch == '"') {
            if (!inside_dquote) {
                inside_dquote = true;
                escaped_field = true;
            } else {
                if (reader.peek() == '"') {
                    //Still inside dquote
                    reader.pop();
                } else {
                    //We are out of dquote
                    inside_dquote = false;
                }
            }
            
            continue;
        }
        
        if (inside_dquote) {
            continue;
        }
        
        if (ch == ',') {
            reader.putback();
            reader.mark_end();
            field = extract_segment(reader, escaped_field);
            reader.pop();
            
            return HAS_MORE_FIELDS;
        }
        
        if (ch == '\r') {
            reader.putback();
            reader.mark_end();
            field = extract_segment(reader, escaped_field);
            reader.pop();
            reader.pop(); //Read \n
            
            return END_RECORD;
        }
        
        /*
         * Non-standard end of line with just a \n
         */
        if (ch == '\n') {
            reader.putback();
            reader.mark_end();
            field = extract_segment(reader, escaped_field);
            reader.pop();
            
            return END_RECORD;
        }
    }
}

ParseStatus parse_record(Reader& reader, std::span<std::string_view> storage, std::span<std::string_view>& record) {
    size_t field_index = 0;
    std::string_view field;
    
    while (true) {
        auto status = next_field(reader, field);
        
        if (status == END_DOCUMENT) {
            return status;
        }
        
        if (field_index < storage.size()) {
            storage[field_index] = field;

            ++field_index;
        }
        
        if (status == END_RECORD) {
            record = storage.subspan(0, field_index);
            
            return status;
        }        
    }
    
    assert(false);
}

void _parse(Reader& reader, std::span<std::string_view> storage, std::function<void(size_t, std::span<std::string_view>)> consumer) {
    std::span<std::string_view> record;
    size_t index = 0;
    
    while (parse_record(reader, storage, record) != END_DOCUMENT) {
        consumer(index, record);
        
        ++index;
    }
}

}

