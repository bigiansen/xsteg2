#include <xsteg/threshold.hpp>

#include <ien/image_ops.hpp>
#include <ien/strutils.hpp>
#include <xsteg/visual_data.hpp>

#include <array>
#include <charconv>
#include <sstream>

namespace xsteg
{
	uint8_t truncate_threshold_value_u8(uint8_t val, const pixel_availability& av)
	{
		uint8_t max_availability = 0;
		max_availability = av.ignore_r ? max_availability : std::max(av.bits_r, max_availability);
		max_availability = av.ignore_g ? max_availability : std::max(av.bits_g, max_availability);
		max_availability = av.ignore_b ? max_availability : std::max(av.bits_b, max_availability);
		max_availability = av.ignore_a ? max_availability : std::max(av.bits_a, max_availability);
		return val & (0xFF << max_availability);
	}	

	float truncate_threshold_value_f32(float val, const pixel_availability& av)
	{
		uint8_t max_availability = 0;
		max_availability = av.ignore_r ? max_availability : std::max(av.bits_r, max_availability);
		max_availability = av.ignore_g ? max_availability : std::max(av.bits_g, max_availability);
		max_availability = av.ignore_b ? max_availability : std::max(av.bits_b, max_availability);
		max_availability = av.ignore_a ? max_availability : std::max(av.bits_a, max_availability);

		float mod = ((float)(1 << 7)) / 256.0F;
		return val - std::fmod(val, mod);
	}

    void apply_threshold_chcmp(ien::fixed_vector<pixel_availability>& map, const ien::img::image& img, const threshold& th)
    {
        ien::img::rgba_channel channel;
        switch(th.type)
        {
            case visual_data_type::CHANNEL_RED:
                channel = ien::img::rgba_channel::R;
                break;
            case visual_data_type::CHANNEL_GREEN:
                channel = ien::img::rgba_channel::G;
                break;
            case visual_data_type::CHANNEL_BLUE:
                channel = ien::img::rgba_channel::B;
                break;
            case visual_data_type::CHANNEL_ALPHA:
                channel = ien::img::rgba_channel::A;
                break;
            default:
                throw std::logic_error("Invalid channel type");
        }

        uint8_t thres_val = static_cast<uint8_t>(th.value * 255.0F);
		thres_val = truncate_threshold_value_u8(thres_val, th.availability);

        ien::fixed_vector<uint8_t> res = ien::img::channel_compare(img, channel, thres_val);
        for(size_t i = 0; i < img.pixel_count(); ++i)
        {
            if(res[i] ^ static_cast<uint8_t>(th.inverted))
            {
                map[i].merge(th.availability);
            }
        }
    }

    void apply_threshold_generic(ien::fixed_vector<pixel_availability>& map, const ien::img::image& img, const threshold& th)
    {
        ien::fixed_vector<float> vdata = extract_visual_data(img, th.type);
		float thres_val = truncate_threshold_value_f32(th.value, th.availability);
        for(size_t i = 0; i < img.pixel_count(); ++i)
        {
            if((vdata[i] >= thres_val) ^ th.inverted)
            {
                map[i].merge(th.availability);
            }
        }
    }

    void apply_threshold(ien::fixed_vector<pixel_availability>& map, const ien::img::image& img, const threshold& th)
    {
        ien::img::image img_copy = img;

        const pixel_availability& av = th.availability;
        ien::img::truncate_channel_data(
            img_copy.data(),
            av.ignore_r ? 0 : av.bits_r,
            av.ignore_g ? 0 : av.bits_g,
            av.ignore_b ? 0 : av.bits_b,
            av.ignore_a ? 0 : av.bits_a
        );

        switch(th.type)
        {
            case visual_data_type::CHANNEL_RED:
            case visual_data_type::CHANNEL_GREEN:
            case visual_data_type::CHANNEL_BLUE:
            case visual_data_type::CHANNEL_ALPHA:
                apply_threshold_chcmp(map, img_copy, th);
                return;

            default:
                apply_threshold_generic(map, img_copy, th);
        }
    }

    const char TH_SEG_SEPARATOR     = '-';
    const char TH_SEPARATOR         = '$';
    const char TH_NON_INVERTED      = 'N';
    const char TH_INVERTED          = 'I';
    const char TH_BITS_IGNORED      = 'X';
    const std::array<char, 9> TH_BITS_ALLOWED_CHARS = {
        TH_BITS_IGNORED, '0', '1', '2', '3', '4', '5', '6', '7' 
    };

    std::string gen_single_threshold_key(const threshold& thres)
    {
        // $1-N-013X-0.556
        std::stringstream result;
        result << TH_SEPARATOR
               << std::to_string(static_cast<int>(thres.type))
               << TH_SEG_SEPARATOR
               << (thres.inverted ? TH_INVERTED : TH_NON_INVERTED)
               << TH_SEG_SEPARATOR
               << (thres.availability.ignore_r ? TH_BITS_IGNORED : std::to_string(thres.availability.bits_r)[0])
               << (thres.availability.ignore_g ? TH_BITS_IGNORED : std::to_string(thres.availability.bits_g)[0])
               << (thres.availability.ignore_b ? TH_BITS_IGNORED : std::to_string(thres.availability.bits_b)[0])
               << (thres.availability.ignore_a ? TH_BITS_IGNORED : std::to_string(thres.availability.bits_a)[0])
               << TH_SEG_SEPARATOR
               << std::to_string(thres.value);
        return result.str();
    }

    void validate_key_segments(
        std::string_view type, 
        std::string_view invt, 
        std::string_view bits, 
        std::string_view val)
    {
        ien::runtime_assert(
            std::all_of(type.begin(), type.end(), [](char ch) {
                return std::isdigit(ch);
            }),
            "Invalid type parameter (invalid character(s))"
        );

        ien::runtime_assert(
            invt.size() == 1, 
            "Invalid inversion parameter (invalid size)"
        );

        ien::runtime_assert(
            invt[0] == TH_NON_INVERTED || invt[0] == TH_INVERTED,
            "Invalid inversion parameter (invalid character(s))"
        );

        ien::runtime_assert(
            bits.size() == 4, 
            "Invalid channel bit availability parameter format (invalid size)"
        );

        ien::runtime_assert(std::all_of(bits.begin(), bits.end(), [](char ch) {
                return std::count(TH_BITS_ALLOWED_CHARS.begin(), TH_BITS_ALLOWED_CHARS.end(), ch) == 1;
            }),
            "Invalid channel bit availability parameter format (invalid character(s))"
        );

        float dummy;
        ien::runtime_assert(
            std::from_chars(val.data(), val.data() + val.size(), dummy).ec != std::errc::invalid_argument,
            "Invalid value parameter (bad format)"
        );
    }

    ien::fixed_vector<threshold> parse_thresholds_key(const std::string& key)
    {
        size_t threshold_count = std::count(key.begin(), key.end(), TH_SEPARATOR);
        ien::fixed_vector<threshold> result(threshold_count);

        auto thresholds = ien::strutils::split_view(key, TH_SEPARATOR);
        for(size_t i = 0; i < thresholds.size(); ++i)
        {
            auto th = thresholds[i];

            auto segments = ien::strutils::split_view(th, TH_SEG_SEPARATOR);
            std::string_view sv_type = segments[0];
            std::string_view sv_invt = segments[1];
            std::string_view sv_bits = segments[2];
            std::string_view sv_tval = segments[3];

            validate_key_segments(sv_type, sv_invt, sv_bits, sv_tval);

            int i_type = ien::strutils::string_view_to_integral<int>(sv_type);
            visual_data_type type = static_cast<visual_data_type>(i_type);
            bool inverted = sv_invt[0] == TH_NON_INVERTED ? false : true;
            pixel_availability av(
                sv_bits[0] == TH_BITS_IGNORED ? -1 : (sv_bits[0] - '0'),
                sv_bits[1] == TH_BITS_IGNORED ? -1 : (sv_bits[1] - '0'),
                sv_bits[2] == TH_BITS_IGNORED ? -1 : (sv_bits[2] - '0'),
                sv_bits[3] == TH_BITS_IGNORED ? -1 : (sv_bits[3] - '0')
            );
            float value = ien::strutils::string_view_to_float<float>(sv_tval);

            result[i] = threshold(type, inverted, value, av);
        }
        return result;
    }
}