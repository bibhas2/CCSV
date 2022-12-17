#include "Parser.h"

namespace ccsv {
void eat_space(Reader& reader) {
    char ch = reader.pop();

    if (ch == 0) return;

    while (isspace(ch)) {
        ch = reader.pop();

        if (ch == 0) return;
    }

    reader.putback();
}

bool parse_record(Reader& reader, std::span<std::string_view> storage, std::span<std::string_view>& record) {
    size_t field_index = 0;
    
    while (field_index < storage.size()) {
        reader.mark_start();
        
        while (true) {
            char ch = reader.peek();
            
            if (ch == '\0') {
                //End of data
                return false;
            }
            
            if (ch == ',' || ch == '\r') {
                //End of field.
                reader.mark_end();
                
                storage[field_index] = reader.segment();
                
                //Eat the last char
                reader.pop();
                
                if (ch == ',') {
                    //We have more fields
                    ++field_index;
                    break;
                } else {
                    //Read \n
                    reader.pop();

                    //End of record
                    record = storage.subspan(0, field_index + 1);

                    return true;
                }
            } else {
                reader.pop();
            }
        }
    }
    
    //We should not come here
    assert(false);
    
    return false;
}

void _parse(Reader& reader, std::span<std::string_view> storage, std::function<void(size_t, std::span<std::string_view>)> consumer) {
    std::span<std::string_view> record;
    size_t index = 0;
    
    while (parse_record(reader, storage, record)) {
        consumer(index, record);
        
        ++index;
    }
}

}

