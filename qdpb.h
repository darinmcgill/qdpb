#pragma once
#include <cstdint>
#include <string>
#include <cstring>
#include <vector>
#include <stdexcept>

namespace qdpb
{
    using packed_t = uint64_t;
    using byte_p = char *;

    class Chomper
    {
        std::vector<std::vector<packed_t>> maw_;
        char* start_;
        char* ender_;
        bool mutate_;
        packed_t _get(int field_number);
        bool bite(byte_p &ptr);

    public:

        void chomp(char*, byte_p ender = nullptr, bool mutate=true);
        void chomp(const char*, byte_p ender = nullptr);

        template<class T>
        T get(int field_number) {
            packed_t p = _get(field_number);
            return *reinterpret_cast<T *>(&p);
        }

        bool get(int field_number, byte_p& ptr, size_t& size);

        void unpack(int field_number, int wire_type);
    };
}