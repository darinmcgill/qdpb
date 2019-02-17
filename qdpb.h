#pragma once
#include <cstdint>
#include <string>
#include <cstring>
#include <vector>
#include <stdexcept>

namespace qdpb
{
    using packed_t = uint64_t;
    const uint64_t _mask = ((~0ULL) << 16) >> 16;
    using byte_p = char *;

    inline packed_t pack(const void *p, size_t d = 0)
    { return (reinterpret_cast<packed_t>(p) & _mask) | d << 48; }

    inline size_t get_size(packed_t packed)
    { return static_cast<size_t>((packed >> 48) & 0xffff); }

    inline void *get_ptr(packed_t packed)
    { return reinterpret_cast<void *>(static_cast<int64_t>(packed << 16) >> 16); }


    class Chomper
    {
        std::vector<std::vector<packed_t>> maw;

    public:
        bool bite(byte_p &ptr);

        void chomp(byte_p start, byte_p ender = nullptr);

        void reset();

        template<class T>
        T get(int field_number)
        {
            if (field_number > maw.size())
                return 0;
            std::vector<packed_t> &tooth = maw[field_number - 1];
            if (tooth.empty())
                return 0;
            packed_t p = tooth.back();
            return *reinterpret_cast<T *>(&p);
        }

        bool get(int field_number, byte_p& ptr, size_t& size)
        {
            if (field_number > maw.size())
                return false;
            std::vector<packed_t> &tooth = maw[field_number - 1];
            if (tooth.empty())
                return false;
            packed_t p = tooth.back();
            size = get_size(p);
            ptr = static_cast<byte_p>(get_ptr(p));
            return true;
        }
    };
}