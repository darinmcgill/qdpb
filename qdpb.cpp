#include <iostream>
#include <sstream>
#include "qdpb.h"
using namespace qdpb;
const uint64_t _mask = ((~0ULL) << 16) >> 16;

misc_t squish(const void *p, size_t d = 0)
{ return (reinterpret_cast<misc_t>(p) & _mask) | d << 48; }

size_t get_size(misc_t misc)
{ return static_cast<size_t>((misc >> 48) & 0xffff); }

char* get_ptr(misc_t misc)
{ return reinterpret_cast<char *>(static_cast<int64_t>(misc << 16) >> 16); }


uint64_t read_varint(byte_p &ptr, bool clear) {
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

misc_t read_value(WireType wire_type, byte_p &ptr, bool mutate) {
    uint64_t len;
    misc_t out;
    switch (wire_type) {
        case WireType::VARINT:
            return read_varint(ptr, mutate);
        case WireType::DOUBLE:
            out = *reinterpret_cast<uint64_t*>(ptr);
            ptr += 8;
            break;
        case WireType::PASCAL:
            len = read_varint(ptr, mutate);
            out = squish(ptr, len);
            ptr += len;
            break;
        case WireType::SINGLE:
            out = *reinterpret_cast<uint32_t*>(ptr);
            ptr += 4;
            break;
        default:
            throw std::runtime_error("bad wire_type:" + std::to_string(wire_type));
    }
    return out;
}

bool Parser::read_field(byte_p &ptr) {
    auto one = read_varint(ptr, mutate_);
    if (not one)
        return false;
    auto wire_type = static_cast<WireType>(one & 0x07);
    auto field_number = one >> 3;
    if (vecs_.size() < field_number)
        vecs_.resize(field_number);
    std::vector<misc_t>& dest = vecs_[field_number - 1];
    dest.push_back(read_value(wire_type, ptr, mutate_));
    return true;
}

void Parser::parse(char *ptr, byte_p ender, bool mutate)
{
    for (auto& vec: vecs_)
        vec.clear();
    mutate_ = mutate;
    std::cerr << "mutating chomp:" << mutate << std::endl;
    start_ = ptr;
    while (true) {
        if (ender and ptr >= ender)
            break;
        bool more = read_field(ptr);
        if (not more)
            break;
    }
    ender_ = ptr;
}

void Parser::parse(const char *start, byte_p ender)
{
    std::cerr << "const chomp" << std::endl;
    parse((char *) start, ender, false);
}


bool Parser::get_string(int field_number, byte_p &ptr, size_t &size, int index) const
{
    if (field_number > vecs_.size())
        return false;
    const std::vector<misc_t> &field_vec = vecs_[field_number - 1];
    misc_t p;
    if (index < 0)
    {
        if (field_vec.empty())
            return false;
        p = field_vec.back();
    } else {
        p = field_vec.at((unsigned long) index);
    }
    size = get_size(p);
    ptr = get_ptr(p);
    return true;
}

misc_t Parser::get_misc(int field_number, int index) const
{
    if (field_number > vecs_.size())
        return 0;
    const std::vector<misc_t> &field_vec = vecs_[field_number - 1];
    if (index < 0) {
        if (field_vec.empty())
            return 0;
        return field_vec.back();
    } else {
        return field_vec.at(static_cast<size_t>(index));
    }
}

void Parser::unpack(int field_number, WireType wire_type)
{
    byte_p ptr;
    size_t size;
    bool present = get_string(field_number, ptr, size);
    byte_p end = ptr + size;
    if (not present)
        return;
    if (ptr < start_ or ptr > ender_)
        throw std::runtime_error("can't unpack non-string");
    std::vector<misc_t> &dest = vecs_[field_number - 1];
    dest.clear();
    while (ptr < end) {
        dest.push_back(read_value(wire_type, ptr, mutate_));
    }
}

std::string Parser::dump()
{
    std::stringstream ss;
    ss << "{";
    for (int i=0;i<vecs_.size();i++) {
        auto& vec = vecs_[i];
        if (not vec.empty()) {
            ss << i << ":[";
            for (auto e: vec)
            {
                auto ptr = get_ptr(e);
                auto size = get_size(e);
                if (ptr >= start_ and ptr <= ender_) {
                    ss << "'" << std::string(ptr, size) << "',";
                } else {
                    ss << e << ",";
                }

            }
            ss << "],";
        }
    }
    ss << "}";
    return ss.str();
}
