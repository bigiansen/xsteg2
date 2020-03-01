#include <xsteg/availability.hpp>

#include <ien/bit_tools.hpp>
#include <algorithm>

namespace xsteg
{
    void pixel_availability::merge(const pixel_availability& src)
    {
        if(!src.ignore_r) { bits_r = src.bits_r; ignore_r = false; }
        if(!src.ignore_g) { bits_g = src.bits_g; ignore_g = false; }
        if(!src.ignore_b) { bits_b = src.bits_b; ignore_b = false; }
        if(!src.ignore_a) { bits_a = src.bits_a; ignore_a = false; }
    }

    availability_map::availability_map(size_t len)
        : fixed_vector(len)
    {
        std::fill(begin(), end(), pixel_availability(-1, -1, -1, -1));
    }
}