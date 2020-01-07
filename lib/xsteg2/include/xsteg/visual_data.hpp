#pragma once

#include <xsteg/visual_data_type.hpp>

#include <ien/fixed_vector.hpp>
#include <ien/image.hpp>

#include <cinttypes>

namespace xsteg
{
    ien::fixed_vector<float> extract_visual_data(const ien::image& img, visual_data_type type);
}