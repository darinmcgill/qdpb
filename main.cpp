#include <iostream>
#include <cstring>
#include <fstream>
#include <memory>
#include <cstdlib>
#include <sstream>
#include <iomanip>
#include "qdpb.h"

#define VERIFY(x) ((x) or barf(#x, __FILE__, __LINE__))

inline bool barf(const char *cond_string, const char *fn, int line) {
    throw std::runtime_error(std::string("failed VERIFY: ") + cond_string + " " + fn + " " + std::to_string(line));
}


int main(int argc, char** argv)
{
    using namespace std;
    using namespace qdpb;

    if (argc > 1) {
        Parser parser;
        ifstream handle(argv[1], ios::binary | ios::in | ios::ate);
        VERIFY(handle.is_open());
        long size = handle.tellg();
        string s(size, 0);
        auto ptr = (char*) s.c_str();
        handle.seekg(0, ios::beg);
        handle.read(ptr, size);
        handle.close();
        parser.parse((const char*) ptr, ptr+size);
        cout << parser.dump() << endl;
        return 0;
    }


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
    qdpb::Parser chomper;
    chomper.parse(m4);
    char* c;
    size_t s;
    auto hit = chomper.get_string(2, c, s);
    cout << "hit: " << hit << endl;
    cout << "=>" << std::string(c, s) << "<=" << endl;
    cout << chomper.get_value<int>(1) << endl;
    string s4(m4);
    auto m5 = (char*) s4.c_str();
    chomper.parse(m5);
    hit = chomper.get_string(2, c, s);
    cout << "hit: " << hit << endl;
    cout << "=>" << c << "<=" << endl;
    cout << chomper.get_value<int>(1) << endl;
    chomper.unpack(3,qdpb::WireType::VARINT);
    cout << chomper.get_value<int>(3) << endl;
    cout << chomper.get_value<int>(3, 1) << endl;
    cout << chomper.dump() << endl;
}