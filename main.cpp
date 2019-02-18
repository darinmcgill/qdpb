#include <iostream>
#include <cstring>
#include "qdpb.h"

char* leak_copy(const char* ptr, const char* end= nullptr) {
    if (not end)
        end = strlen(ptr) + ptr;
    size_t size = end - ptr;
    auto out = (char*) malloc(size + 1);
    *(out + size) = 0;
    memcpy(out, ptr, end - ptr);
    return out;
}

int main()
{
    using namespace std;
    /*
    auto msg = "Hello, World!";
    cout.write("=>", 2);
    auto len = strlen(msg);
    cout.write(msg, len);
    cout.write("<=\n", 3);

    auto p = pack(msg, len);
    cout << "=>" << as<string>(p) << "<=" << endl;
    cout << "=>" << as<char*>(p) << "<=" << endl;
    cout << "=>" << as<int64_t>(p) << "<=" << endl;
    cout << endl;
     */

    auto m2 = (char*) "\xac\x02\xad\x02";
    auto m3 = (char*) "\x12\x07\x74\x65\x73\x74\x69\x6e\x67";
    auto m4 = "\x12\x07\x74\x65\x73\x74\x69\x6e\x67\x08\x07\x1a\x03\x11\x12\x13";
    qdpb::Chomper chomper;
    chomper.chomp(m4);
    char* c;
    size_t s;
    auto hit = chomper.get(2, c, s);
    cout << "hit: " << hit << endl;
    cout << "=>" << std::string(c, s) << "<=" << endl;
    cout << chomper.get<int>(1) << endl;
    chomper.reset();
    auto m5 = leak_copy(m4);
    chomper.chomp(m5);
    hit = chomper.get(2, c, s);
    cout << "hit: " << hit << endl;
    cout << "=>" << c << "<=" << endl;
    cout << chomper.get<int>(1) << endl;
    chomper.unpack(3,0,true);
    cout << chomper.get<int>(3) << endl;
}