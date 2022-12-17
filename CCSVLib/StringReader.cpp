#include "StringReader.h"

namespace ccsv {
    StringReader::StringReader(std::string_view source) : Reader(), data(source) {
    }

    StringReader::StringReader() : Reader() {
    }

    StringReader::~StringReader() {
        data = {};
        position = 0;
    }

    char StringReader::peek() {
        if (position < data.size()) {
            return data.at(position);
        }
        else {
            return '\0';
        }
    }

    char StringReader::pop() {
        if (position < data.size()) {
            char result = data.at(position);

            ++position;

            return result;
        }
        else {
            return '\0';
        }
    }

    void StringReader::putback() {
        if (position > 0) {
            --position;
        }
    }

    void StringReader::mark_start() {
        start = position;
    }

    void StringReader::mark_end() {
        end = position;
    }

    std::string_view StringReader::segment() {
        return data.substr(start, end - start);
    }

    bool StringReader::good() {
        return data.data() != nullptr;
    }
}
