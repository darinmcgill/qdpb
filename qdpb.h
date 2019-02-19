#pragma once
#include <cstdint>
#include <string>
#include <cstring>
#include <vector>
#include <stdexcept>

namespace qdpb
{
    using misc_t = uint64_t;
    using byte_p = char *;

    enum WireType {
        VARINT=0,
        DOUBLE=1,
        PASCAL=2,
        SINGLE=5
    };

    class Parser
    {
        std::vector<std::vector<misc_t>> vecs_;
        char* start_;
        char* ender_;
        bool mutate_;
        misc_t get_misc(int field_number, int index=-1) const;
        bool read_field(byte_p &ptr);

    public:

        void parse(char *ptr, byte_p ender = nullptr, bool mutate = true);
        void parse(const char *ptr, byte_p ender = nullptr);

        template<class T>
        T get_value(int field_number, int index=-1) const {
            misc_t p = get_misc(field_number, index);
            return *reinterpret_cast<T *>(&p);
        }

        bool get_string(int field_number, byte_p &ptr, size_t &size, int index=-1) const;

        void unpack(int field_number, WireType wire_type);

        std::string dump();
    };
}