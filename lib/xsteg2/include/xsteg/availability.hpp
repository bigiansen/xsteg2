#pragma once

#include <ien/assert.hpp>
#include <ien/debug.hpp>
#include <ien/fixed_vector.hpp>
#include <cinttypes>

namespace xsteg
{
    struct pixel_availability
    {
        uint8_t bits_r : 3;
        bool ignore_r  : 1;

        uint8_t bits_g : 3;
        bool ignore_g  : 1;

        uint8_t bits_b : 3;
        bool ignore_b  : 1;

        uint8_t bits_a : 3;
        bool ignore_a  : 1;

        LIEN_RELEASE_ONLY(constexpr) pixel_availability(int8_t r, int8_t g, int8_t b, int8_t a)
            : bits_r(static_cast<uint8_t>(r < 0 ? 0 : r))
            , ignore_r(r < 0)
            , bits_g(static_cast<uint8_t>(g < 0 ? 0 : g))
            , ignore_g(g < 0)
            , bits_b(static_cast<uint8_t>(b < 0 ? 0 : b))
            , ignore_b(b < 0)
            , bits_a(static_cast<uint8_t>(a < 0 ? 0 : a))
            , ignore_a(a < 0)
        {
            LIEN_DEBUG_ASSERT_MSG(r < 8, "Max availability is 7 bits per channel!");
            LIEN_DEBUG_ASSERT_MSG(g < 8, "Max availability is 7 bits per channel!");
            LIEN_DEBUG_ASSERT_MSG(b < 8, "Max availability is 7 bits per channel!");
            LIEN_DEBUG_ASSERT_MSG(a < 8, "Max availability is 7 bits per channel!");
        }

        void merge(const pixel_availability& src);
    };
    
    static_assert(sizeof(pixel_availability) == 2);

    struct availability_map : public ien::fixed_vector<pixel_availability>
    {
        availability_map(size_t len);
    };
}