#include <iostream>
#include "qdpb.h"


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
    qdpb::Chomper chomper;
    chomper.chomp(m3);
    char* m4;
    size_t s;
    auto hit = chomper.get(2, m4, s);
    cout << "hit: " << hit << endl;
    cout << "=>" << std::string(m4, s) << "<=" << endl;
    chomper.reset();
}