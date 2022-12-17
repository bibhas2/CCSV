#pragma once

#include "Parser.h"

namespace ccsv {
    struct StringReader : public ccsv::Reader
    {
        std::string_view data;

        StringReader();
        StringReader(std::string_view source);

        char peek();
        char pop();
        void putback();
        bool good();
        void mark_start();
        void mark_end();
        std::string_view segment();

        virtual ~StringReader();
    };
}
