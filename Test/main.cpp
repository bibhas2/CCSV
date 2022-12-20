#include <StringReader.h>
#include <Parser.h>
#include <assert.h>
#include <iostream>
#include <MemoryMappedReader.h>
#include <fstream>

void test_record() {
    auto str =
        "aa,bb,cc,dd\r\n"
        "ee,ff,gg,hh\r\n";
    
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
    auto str =
        "aa,bb,cc,dd\r\n"
        "\r\n"
        "ee,ff,gg,hh\r\n";
    
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
 * For escaped fields, spaces before or after "" not allowed in theRFC.
 * But many CSV files may have it.
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

void test_line_feed() {
    //Non-standard CSV but common in Linux/macOS
    auto str =
        "aa,bb,cc,dd\n"
        "ee,ff,gg,hh\n";
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
    auto str =
        "aa,bb,cc,dd\r\n" //More fields than storage
        "ee,ff,gg\r\n"
        "hh,ii\r\n"; //Less fields than storage
    
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
    assert(reader.pop() == ',');
    
    reader.mark_stop();
    
    assert(reader.segment() == "bb");
}

void test_memory_map_reader() {
    auto str =
        "aa,bb,cc,dd\r\n"
        "ee,ff,gg\r\n"
        "hh,ii\r\n";

    const char* file_name = "__test.csv";

    {
        std::ofstream test_file(file_name);

        test_file << str;
    } //Closes file

    {
        ccsv::MemoryMappedReader reader(file_name);

        assert(reader.good());
        assert(reader.data.size() > 0);

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
    } //Closes file

    std::remove(file_name);
}

int main() {
    test_string_reader();
    test_record();
    test_uneven();
    test_line_feed();
    test_basic_escape();
    test_empty_line();
    test_space();
    test_memory_map_reader();
    
    return 0;
}
