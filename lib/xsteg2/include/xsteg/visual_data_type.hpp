#pragma once

#include <cinttypes>

namespace xsteg
{
    enum class visual_data_type : uint8_t
    {
        CHANNEL_RED = 0,
        CHANNEL_GREEN = 1,
        CHANNEL_BLUE = 2,
        CHANNEL_ALPHA = 3,
        AVERAGE_RGB = 4,
        AVERAGE_RGBA = 5,
        SATURATION = 6,
        LUMINANCE = 7,
        CHANNEL_SUM_SATURATED = 8
    };
}