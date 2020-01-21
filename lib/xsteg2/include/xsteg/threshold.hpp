#pragma once

#include <xsteg/availability.hpp>
#include <xsteg/visual_data_type.hpp>

#include <ien/debug.hpp>
#include <ien/fixed_vector.hpp>
#include <ien/image.hpp>

#include <sstream>

namespace xsteg
{
    struct threshold
    {
        visual_data_type type;
        bool inverted;
        float value;
        pixel_availability availability;

        LIEN_RELEASE_CONSTEXPR threshold(visual_data_type vdtype, bool inv, float val, pixel_availability pxav)
            : type(vdtype)
            , inverted(inv)
            , value(val)
            , availability(pxav)
        { }
    };

    void apply_threshold(ien::fixed_vector<pixel_availability>& map, const ien::image& img, const threshold& th);

    std::string gen_single_threshold_key(const threshold&);

    template<typename T>
    std::string gen_thresholds_key(const T& iterable)
    {
        static_assert(std::is_same_v<typename T::value_type, threshold>, "Not a threshold container!");
        if(iterable.begin() == iterable.end())
        {
            return std::string();
        }

        std::stringstream result;
        for(const threshold& t : iterable)
        {
            result << gen_single_threshold_key(t);
        }
        return result.str();
    }

    ien::fixed_vector<threshold> parse_thresholds_key(const std::string& key);
}