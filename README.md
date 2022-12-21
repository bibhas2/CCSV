# Non-Allocating CSV Parser in C++
Key features of this library.

- Follows [RFC 4180](https://www.rfc-editor.org/rfc/rfc4180).
- Does not allocate any memory on the heap.
- Uses the latest non-allocating and non-owned STL data structures like ``std::string_view`` and ``std::span`` to simplify code.

## Quick Example
```c
#include <StringReader.h>
#include <Parser.h>
#include <assert.h>

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
```

The ``ccsv::parse<10>()`` call statically allocates enough space for 10 fields per line. Any excess fileds are discarded and does not cause any errors.

# Building

## Using Makefile
From the root of the project run.

```
make
```

This will build the library and run the unit tests. If all goes well you should see:

```
Running tests...
All tests passed.
```

## Using Xcode
Open the ``CCSV`` workspace. Then run the unit tests (Command+R). This will build the library, the unit tests and run the tests.

## Using Visual Studio

TBD.

# User Guide
## Parsing a String

Use the ``StringReader`` class to read from a string. You need to have an estimate for how many fields are expected per line. This is needed to statically allocate space at compile time. You can always err on the side of caution. For example, if you expect 4 fields per line, you can configure the parser with 64 fields.

```c
void test_record() {
    auto str =
        "aa,bb,cc,dd\r\n"
        "ee,ff,gg,hh\r\n";
    
    ccsv::StringReader reader(str);

    ccsv::parse<64>(reader, [](size_t index, std::span<std::string_view> fields) {
        std::cout << "Record no:" << index << std::endl;
        std::cout << "Field count:" << fields.size() << std::endl;
    });
}
```

The ``ccsv::parse()`` function receives two parameters.

1. The reader.
2. A lambda that is called for each record.

The lambda receives two parameters:

1. The index of the record. The first line has an index of 0.
2. An array of fields. Each field is represented by a ``std::string_view``. 

If a record has more fields than the parser was configured for then the excess fields are discarded and not reported to the lambda.

```c
void test_uneven() {
    auto str =
        "aa,bb,cc,dd\r\n" //More fields than storage
        "ee,ff,gg\r\n"
        "hh,ii\r\n"; //Less fields than storage
    
    ccsv::StringReader reader(str);

    ccsv::parse<3>(reader, [](auto index, auto fields) {      
        if (index == 0) {
            assert(fields.size() == 3);
        } else if (index == 1) {
            assert(fields.size() == 3);
        } else {
            assert(fields.size() == 2);
        }
    });
}
```

## Parsing a CSV File
Memory mapping is used to read from a CSV file.

Let's suppose you have a file called ``test.csv`` as follows.

```
aa,bb,cc,dd
ee,ff,gg
hh,ii
```

We can read the file like this.

```c
#include <Parser.h>
#include <assert.h>
#include <MemoryMappedReader.h>

void test_memory_map_reader() {
    ccsv::MemoryMappedReader reader("test.csv");

    //Check if file could be mapped
    assert(reader.good());

    ccsv::parse<10>(reader, [](auto index, auto fields) {
        assert(index < 3);
        
        if (index == 0) {
            assert(fields.size() == 4);
            
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
```

# Standard Conformance
The library conforms to RFC 4180. It relaxes the standard a bit to be more flexible.

## UNIX Newline
RFC 4180 requires each line to be ended by CRLF (``\r\n``). It is common in Linux and macOS for files to end with just a LF. The library tolerates such files.

## Spaces Around Escaped Fields
The RFC makes clear that spaces are a part of the fields. They should not be ignored. However, it's not clear what happens to the spaces before or after the double quotes of an escaped field. The ABNF grammer appears to indicate that there should be no spaces. The parser discards spaces before and after the double quotes around an escaped field.

In the example below the field ``" dd "`` has spaces around it. Those spaces are ignored.

```c
void test_space() {
    auto str =
R"( aa, "bb",  cc ,
  " dd "  , " ee "
)";
    ccsv::StringReader reader(str);

    ccsv::parse<10>(reader, [](auto index, auto fields) {
        assert(index < 2);
        
        if (index == 0) {
            assert(fields[0] == " aa");
            assert(fields[1] == "bb");
            assert(fields[2] == "  cc ");
        } else {
            assert(fields[0] == " dd ");
            assert(fields[1] == " ee ");
        }
    });
}
```

## Un-Escaping Double Quotes

Escaped double quotes are not unescaped by the parser. I found no simple way of doing that without allocating. In the example below the field ``"b""b"`` is reported to the lambda without unescaping the double quote.

```c
void test_basic_escape() {
    auto str = R"(aa,"b""b",cc,"d,d"
"ee",ff,"g
g",hh
)";
    ccsv::StringReader reader(str);

    ccsv::parse<10>(reader, [](auto index, auto fields) {
        assert(index < 2);
        
        if (index == 0) {
            //Double quote is not unescaped!
            assert(fields[1] == "b\"\"b");
        }
    });
}
```

