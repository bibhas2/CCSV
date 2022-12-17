#include <StringReader.h>
#include <Parser.h>
#include <assert.h>

void test_lambda() {
    auto str = "aa,bb,cc,dd";
    ccsv::StringReader reader(str);

    ccsv::parse<10>(reader, [](std::span<std::string_view> fields) {
        
    });
    
    //ccsv::parse<10>(reader, <#std::function<void (std::string_view *, size_t)> consumer#>)
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
    test_lambda();
    
    return 0;
}
