#include <iostream>
#include "qdpb.h"
using namespace qdpb;
const uint64_t _mask = ((~0ULL) << 16) >> 16;

packed_t pack(const void *p, size_t d = 0)
{ return (reinterpret_cast<packed_t>(p) & _mask) | d << 48; }

size_t get_size(packed_t packed)
{ return static_cast<size_t>((packed >> 48) & 0xffff); }

char* get_ptr(packed_t packed)
{ return reinterpret_cast<char *>(static_cast<int64_t>(packed << 16) >> 16); }


uint64_t devar(byte_p& ptr, bool clear) {
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

bool Chomper::bite(byte_p& ptr) {
    auto one = devar(ptr, mutate_);
    if (not one)
        return false;
    auto wire_type = one & 0x07;
    auto field_number = one >> 3;
    if (maw_.size() < field_number)
        maw_.resize(field_number);
    std::vector<packed_t>& tooth = maw_[field_number - 1];
    uint64_t len;
    switch (wire_type) {
        case 0:
            tooth.push_back(devar(ptr, mutate_));
            break;
        case 1:
            tooth.push_back(*reinterpret_cast<uint64_t*>(ptr));
            ptr += 8;
            break;
        case 2:
            len = devar(ptr, mutate_);
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

void Chomper::chomp(char* ptr, byte_p ender, bool mutate)
{
    for (auto& vec: maw_)
        vec.clear();
    mutate_ = mutate;
    std::cerr << "mutating chomp:" << mutate << std::endl;
    start_ = ptr;
    while (true) {
        if (ender and ptr >= ender)
            break;
        bool more = bite(ptr);
        if (not more)
            break;
    }
    ender_ = ptr;
}

void Chomper::chomp(const char* start, byte_p ender)
{
    std::cerr << "const chomp" << std::endl;
    chomp((char*) start, ender, false);
}


bool Chomper::get(int field_number, byte_p &ptr, size_t &size)
{
    if (field_number > maw_.size())
        return false;
    std::vector<packed_t> &tooth = maw_[field_number - 1];
    if (tooth.empty())
        return false;
    packed_t p = tooth.back();
    size = get_size(p);
    ptr = get_ptr(p);
    return true;
}

packed_t Chomper::_get(int field_number)
{
    if (field_number > maw_.size())
        return 0;
    std::vector<packed_t> &tooth = maw_[field_number - 1];
    if (tooth.empty())
        return 0;
    return tooth.back();
}

void Chomper::unpack(int field_number, int wire_type)
{
    byte_p ptr;
    size_t size;
    bool has = get(field_number, ptr, size);
    if (not has)
        return;
    if (ptr < start_ or ptr > ender_)
        throw std::runtime_error("can't unpack");
    throw std::runtime_error("not implemented");
}
