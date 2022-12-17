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
    
    return 0;
}
