#include <StringReader.h>
#include <Parser.h>
#include <assert.h>
#include <iostream>

void test_record() {
    auto str = "aa,bb,cc,dd\r\nee,ff,gg,hh\r\n";
    ccsv::StringReader reader(str);

    ccsv::parse<10>(reader, [](size_t index, std::span<std::string_view> fields) {
        assert(index < 2);
        
        if (index == 0) {
            assert(fields[0] == "aa");
            assert(fields[3] == "dd");
        } else {
            assert(fields[0] == "ee");
            assert(fields[3] == "hh");
        }
    });
}

void test_empty_line() {
    auto str = "aa,bb,cc,dd\r\n\r\nee,ff,gg,hh\r\n";
    ccsv::StringReader reader(str);

    ccsv::parse<10>(reader, [](size_t index, std::span<std::string_view> fields) {
        assert(index < 3);
        
        if (index == 0) {
            assert(fields[0] == "aa");
            assert(fields[3] == "dd");
        } else if (index == 1) {
            //Empty line
            assert(fields.size() == 1);
            assert(fields[0] == "");
        } else {
            assert(fields[0] == "ee");
            assert(fields[3] == "hh");
        }
    });
}

void test_basic_escape() {
    auto str = R"(aa,"b""b",cc,"d,d"
"ee",ff,"g
g",hh
)";
    ccsv::StringReader reader(str);

    ccsv::parse<10>(reader, [](size_t index, std::span<std::string_view> fields) {
        assert(index < 2);
        
        if (index == 0) {
            assert(fields[1] == "b\"\"b");
            assert(fields[3] == "d,d");
        } else {
            assert(fields[0] == "ee");
            assert(fields[2] == "g\ng");
        }
    });
}

/*
 * For unescaped fields, spaces are part of the field and can not be discarded.
 * Spaces before or after "" not allowed in theRFC.
 * But many may CSV files may have it.
 */
void test_space() {
    auto str = R"( aa, "bb",  cc ,
  " cc ", " dd "
)";
    ccsv::StringReader reader(str);

    ccsv::parse<10>(reader, [](size_t index, std::span<std::string_view> fields) {
        assert(index < 2);
        
        if (index == 0) {
            assert(fields[0] == " aa");
            assert(fields[1] == "bb");
            assert(fields[2] == "  cc ");
        } else {
            assert(fields[0] == " cc ");
            assert(fields[1] == " dd ");
        }
    });
}

void test_newline() {
    //Non-standard CSV but common in Linux.macOS
    auto str = "aa,bb,cc,dd\nee,ff,gg,hh\n";
    ccsv::StringReader reader(str);

    ccsv::parse<10>(reader, [](size_t index, std::span<std::string_view> fields) {
        assert(index < 2);
        
        if (index == 0) {
            assert(fields[0] == "aa");
            assert(fields[3] == "dd");
        } else {
            assert(fields[0] == "ee");
            assert(fields[3] == "hh");
        }
    });
}

void test_uneven() {
    auto str = "aa,bb,cc,dd\r\nee,ff,gg\r\nhh,ii\r\n";
    ccsv::StringReader reader(str);

    ccsv::parse<3>(reader, [](size_t index, std::span<std::string_view> fields) {
        assert(index < 3);
        
        if (index == 0) {
            assert(fields.size() == 3);
            
            assert(fields[0] == "aa");
            assert(fields[2] == "cc");
        } else if (index == 1) {
            assert(fields.size() == 3);
            
            assert(fields[0] == "ee");
            assert(fields[1] == "ff");
        } else {
            assert(fields.size() == 2);
            
            assert(fields[0] == "hh");
            assert(fields[1] == "ii");
        }
    });
}

void test_string_reader() {
    auto str = "aa,bb,cc,dd";
    ccsv::StringReader reader(str);
    
    assert(reader.good());
    
    assert(reader.pop() == 'a');
    assert(reader.pop() == 'a');
    assert(reader.peek() == ',');
    assert(reader.pop() == ',');
    
    reader.mark_start();
    
    assert(reader.pop() == 'b');
    assert(reader.pop() == 'b');
    
    reader.mark_end();
    
    assert(reader.segment() == "bb");
}

int main() {
    test_string_reader();
    test_record();
    test_uneven();
    test_newline();
    test_basic_escape();
    test_empty_line();
    test_space();
    
    return 0;
}
