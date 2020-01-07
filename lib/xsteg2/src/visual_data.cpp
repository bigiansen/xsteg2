#include <xsteg/visual_data.hpp>

#include <ien/image_ops.hpp>
#include <ien/rgba_channel.hpp>

using namespace ien;

namespace xsteg
{
    fixed_vector<float> cast_u8vec_to_f32vec(const fixed_vector<uint8_t>& src)
    {
        fixed_vector<float> result(src.size());
        for(size_t i = 0; i < src.size(); ++i)
        {
            result[i] = static_cast<float>(src[i]) / 255.0F;
        }
        return result;
    }

    fixed_vector<float> extract_channel(const img::image& img, img::rgba_channel ch)
    {
        auto* data = img.cdata();
        switch(ch)
        {
            case img::rgba_channel::R:
                return cast_u8vec_to_f32vec(*data->cdata_r());

            case img::rgba_channel::G:
                return cast_u8vec_to_f32vec(*data->cdata_g());

            case img::rgba_channel::B:
                return cast_u8vec_to_f32vec(*data->cdata_b());

            case img::rgba_channel::A:
                return cast_u8vec_to_f32vec(*data->cdata_a());

            default:
                throw std::logic_error("Invalid channel enum type");
        }
    }

    fixed_vector<float> extract_visual_data(const img::image& img, visual_data_type type)
    {
        switch(type)
        {
            case visual_data_type::CHANNEL_RED:
                return extract_channel(img, img::rgba_channel::R);

            case visual_data_type::CHANNEL_GREEN:
                return extract_channel(img, img::rgba_channel::G);

            case visual_data_type::CHANNEL_BLUE:
                return extract_channel(img, img::rgba_channel::B);

            case visual_data_type::CHANNEL_ALPHA:
                return extract_channel(img, img::rgba_channel::A);

            case visual_data_type::AVERAGE_RGB:
                return cast_u8vec_to_f32vec(img::rgb_average(img));

            case visual_data_type::AVERAGE_RGBA:
                return cast_u8vec_to_f32vec(img::rgba_average(img));

            case visual_data_type::SATURATION:
                return img::rgb_saturation(img);

            case visual_data_type::LUMINANCE:
                return img::rgb_luminance(img);

            case visual_data_type::CHANNEL_SUM_SATURATED:
                return cast_u8vec_to_f32vec(img::rgba_sum_saturated(img));

            default:
                throw std::logic_error("Invalid visual_data_type enum type");
        }
    }
}