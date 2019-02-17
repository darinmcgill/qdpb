#include "qdpb.h"
using namespace qdpb;

uint64_t devar(byte_p& ptr, bool clear=false) {
    uint64_t out = 0;
    int phase = 0;
    while (true) {
        auto one = *ptr;
        if (clear)
            *ptr = 0;
        ptr++;
        auto val = one & 0x7f;
        auto car = one & 128;
        out += val << phase;
        if (not car)
            break;
        phase += 7;
    }
    return out;
}

void Chomper::reset() {
    for (auto& vec: maw)
        vec.clear();
}

bool Chomper::bite(byte_p& ptr) {
    auto one = devar(ptr);
    if (not one)
        return false;
    auto wire_type = one & 0x07;
    auto field_number = one >> 3;
    if (maw.size() < field_number)
        maw.resize(field_number);
    std::vector<packed_t>& tooth = maw[field_number - 1];
    uint64_t len;
    switch (wire_type) {
        case 0:
            tooth.push_back(devar(ptr));
            break;
        case 1:
            tooth.push_back(*reinterpret_cast<uint64_t*>(ptr));
            ptr += 8;
            break;
        case 2:
            len = devar(ptr);
            tooth.push_back(pack(ptr, len));
            ptr += len;
            break;
        case 5:
            tooth.push_back(*reinterpret_cast<uint32_t*>(ptr));
            ptr += 4;
            break;
        default:
            throw std::runtime_error("bad wire_type:" + std::to_string(wire_type));
    }
    return true;
}

void Chomper::chomp(byte_p start, byte_p ender)
{
    while (true) {
        if (ender and start >= ender)
            break;
        bool more = bite(start);
        if (not more)
            break;
    }
}
