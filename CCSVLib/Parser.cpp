#include "Parser.h"
#include <ctype.h>
#include <assert.h>

namespace ccsv {

char Parser::peek(std::string_view data) {
    if (position < data.size()) {
        return data.at(position);
    }
    else {
        return '\0';
    }
}

char Parser::pop(std::string_view data) {
    if (position < data.size()) {
        char result = data.at(position);

        ++position;

        return result;
    }
    else {
        return '\0';
    }
}

void Parser::putback() {
    if (position > 0) {
        --position;
    }
}

void Parser::mark_start() {
    start = position;
}

void Parser::mark_stop() {
    stop = position > 0 ? (position - 1) : 0;
}

std::string_view Parser::segment(std::string_view data) {
    return data.substr(start, stop - start);
}

ParseStatus Parser::next_field(std::string_view data, std::string_view& field) {
    bool inside_dquote = false;
    bool escaped_field = false;
    
    mark_start();
    
    while (true) {
        char ch = pop(data);
        
        if (ch == '\0') {
            return END_DOCUMENT;
        }
        
        if (ch == '"') {
            if (!inside_dquote) {
                inside_dquote = true;
                escaped_field = true;
                
                mark_start();
            } else {
                if (peek(data) == '"') {
                    //Still inside dquote
                    pop(data);
                } else {
                    //We are out of dquote
                    inside_dquote = false;
                    
                    mark_stop();
                }
            }
            
            continue;
        }
        
        if (inside_dquote) {
            continue;
        }
        
        if (ch == ',') {
            if (!escaped_field) {
                mark_stop();
            }
            
            field = segment(data);
            
            return HAS_MORE_FIELDS;
        }
        
        if (ch == '\r') {
            if (!escaped_field) {
                mark_stop();
            }
            
            field = segment(data);
            
            pop(data); //Read the LF \n
            
            return END_RECORD;
        }
        
        /*
         * Non-standard end of line with just a LF \n
         */
        if (ch == '\n') {
            if (!escaped_field) {
                mark_stop();
            }

            field = segment(data);
            
            return END_RECORD;
        }
    }
}

ParseStatus Parser::parse_record(std::string_view data, std::span<std::string_view> storage, std::span<std::string_view>& record) {
    size_t field_index = 0;
    std::string_view field;
    
    while (true) {
        auto status = next_field(data, field);
        
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

}

