#include <xsteg/steganographer.hpp>

#include <ien/arithmetic.hpp>
#include <ien/assert.hpp>
#include <ien/bit_tools.hpp>
#include <ien/image_ops.hpp>
#include <ien/multi_array_bit_iterator.hpp>

#include <algorithm>
#include <cstring>

#if defined(XSTEG_STEGANOGRAPHER_HEADER_SIZE) && !defined(XSTEG_STEGANOGRAPHER_HEADER_SIZE_NOWARN)

#warn "Note that using different header sizes WILL break compatibility between different compilations. \
Define XSTEG_STEGANOGRAPHER_HEADER_SIZE_NOWARN to silence this warning"

#endif

#ifndef XSTEG_STEGANOGRAPHER_HEADER_SIZE
    #define XSTEG_STEGANOGRAPHER_HEADER_SIZE 4
#endif

static_assert(
    XSTEG_STEGANOGRAPHER_HEADER_SIZE <= sizeof(std::size_t),
    "XSTEG_STEGANOGRAPHER_HEADER_SIZE must be a value between 1 and sizeof(std::size_t)"
);

namespace xsteg
{
    void default_init_av_map(ien::fixed_vector<pixel_availability>& map)
    {
        std::fill(map.begin(), map.end(), pixel_availability(-1, -1, -1, -1));
    }

    steganographer::steganographer(const char* filename)
        : _img(std::string(filename))
        , _av_map(img().pixel_count())
    { }

    steganographer::steganographer(const std::string& filename)
        : _img(filename)
        , _av_map(img().pixel_count())
    { }

    steganographer::steganographer(const ien::image& image)
        : _img(&image)
        , _av_map(img().pixel_count())
    { }

    void steganographer::add_threshold(const threshold& th, bool apply)
    {
        _thresholds.push_back(th);
        if(apply)
        {
            apply_threshold(_av_map, img(), th);
        }
    }

    void steganographer::add_threshold(threshold&& th, bool apply)
    {
        _thresholds.push_back(std::move(th));
    }

    void steganographer::clear_thresholds()
    {
        clear_availability();
        _thresholds.clear();
        _last_processed_thres_idx = -1;
    }

    void steganographer::clear_availability()
    {
        std::fill(_av_map.begin(), _av_map.end(), xsteg::pixel_availability(-1, -1, -1, -1));
    }

    bool steganographer::apply_next_threshold()
    {
        if(_last_processed_thres_idx++ >= _thresholds.size()) 
        { return false; }

        threshold& th = _thresholds.at(static_cast<signed long long>(_last_processed_thres_idx) - 1);
        apply_threshold(_av_map, img(), th);
        return true;
    }

    void steganographer::apply_thresholds()
    {
        _last_processed_thres_idx = 0;
        while(apply_next_threshold()) { continue; }
    }

    size_t steganographer::available_size_bytes(const encoding_options& opts) const
    {
        if(_thresholds.empty())
            return 0;

        size_t bits = 0;
        size_t pixels = img().pixel_count();
        if(opts.skip_pattern)
        {
            size_t pattern_idx = 0;
            for(size_t i = opts.first_pixel_offset; 
                i < pixels; 
                i += (opts.skip_pattern.value()[pattern_idx++]))
            {
                const auto& av = _av_map[i];
                bits += ien::safe_add<size_t>(
                    av.ignore_r ? 0 : av.bits_r, 
                    av.ignore_g ? 0 : av.bits_g,
                    av.ignore_b ? 0 : av.bits_b, 
                    av.ignore_a ? 0 : av.bits_a
                );
            }
        }
        else
        {
            for(size_t i = opts.first_pixel_offset; i < pixels; ++i)
            {
                const auto& av = _av_map[i];
                bits += ien::safe_add<size_t>(
                    av.ignore_r ? 0 : av.bits_r,
                    av.ignore_g ? 0 : av.bits_g,
                    av.ignore_b ? 0 : av.bits_b,
                    av.ignore_a ? 0 : av.bits_a
                );
            }
        }
		size_t result = bits / 8;
		return result < XSTEG_STEGANOGRAPHER_HEADER_SIZE
			? 0 
			: result - XSTEG_STEGANOGRAPHER_HEADER_SIZE;
    }
    
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ ATTENTION +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // The following code is an absolute mess.
    // There is just too much state floating around to cleanly separate it,
    // so i've kept it separated into lambdas.
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

    ien::image steganographer::encode(const uint8_t* data, size_t len, const encoding_options& opts) const
    {
        LIEN_ASSERT_MSG(
            len > 0,
            "Attempt to encode 0-length data!"
        );

        LIEN_ASSERT_MSG(
            len <= available_size_bytes(opts),
            ("Not enough space available to encode data!  Requested: " +
                std::to_string(len) + " Available: " + std::to_string(available_size_bytes())).c_str()
        );

        ien::image result = img();

        uint8_t* const r = result.data()->data_r() + opts.first_pixel_offset;
        uint8_t* const g = result.data()->data_g() + opts.first_pixel_offset;
        uint8_t* const b = result.data()->data_b() + opts.first_pixel_offset;
        uint8_t* const a = result.data()->data_a() + opts.first_pixel_offset;

        uint8_t size_header_data[XSTEG_STEGANOGRAPHER_HEADER_SIZE];
        for(uint8_t i = 0; i < XSTEG_STEGANOGRAPHER_HEADER_SIZE; ++i)
        {
            size_header_data[i] = static_cast<uint8_t>(len >> (i * 8));
        }

        ien::multi_array_bit_iterator<const uint8_t> data_it;
        data_it.append_view(size_header_data, XSTEG_STEGANOGRAPHER_HEADER_SIZE);
        data_it.append_view(data, len);

        size_t current_pixel_idx = opts.first_pixel_offset;
        size_t skip_pattern_offset = 0;
        auto next_pixel = [&]() -> void
        {
            auto offset = opts.skip_pattern
                ? opts.skip_pattern.value()[skip_pattern_offset]
                : 0;
                
            current_pixel_idx += (offset + 1);
        };

        size_t processed_bits = 0;
        auto next_bits_mask = [&](uint8_t amount) -> uint8_t
        {
            LIEN_DEBUG_ASSERT_MSG(amount <= 8, "Requested too many bits!");
            uint8_t result = 0;
            for(size_t i = 0; i < amount; ++i)
            {
                result |= ((1 * (static_cast<uint8_t>(*data_it)) << i));
                ++data_it;
            }
            return result;
        };

        auto write_bits_channel = [&](uint8_t* ch, bool ignore, uint8_t bits) -> bool
        {
            if(!ignore && bits > 0)
            {
                uint8_t data_bitmask = next_bits_mask(bits);
                ch[current_pixel_idx] &= ~((1 << bits) - 1);
                ch[current_pixel_idx] |= data_bitmask;
                processed_bits += bits;
            }
            return processed_bits < data_it.total_bits();
        };

        // Encode data
        while(processed_bits < data_it.total_bits())
        {
            const pixel_availability& av = _av_map[current_pixel_idx];
            bool ok = true;
                   { ok = write_bits_channel(r, av.ignore_r, av.bits_r); }
            if(ok) { ok = write_bits_channel(g, av.ignore_g, av.bits_g); }
            if(ok) { ok = write_bits_channel(b, av.ignore_b, av.bits_b); }
            if(ok) {      write_bits_channel(a, av.ignore_a, av.bits_a); }
            next_pixel();
        }

        return result;
    }

    ien::fixed_vector<uint8_t> steganographer::decode(const encoding_options& opts) const
    {
		LIEN_ASSERT_MSG(
			available_size_bytes(opts) > XSTEG_STEGANOGRAPHER_HEADER_SIZE,
			"Specified thresholds cannot hold any data!"
		);
        const uint8_t* const r = img().cdata()->cdata_r() + opts.first_pixel_offset;
        const uint8_t* const g = img().cdata()->cdata_g() + opts.first_pixel_offset;
        const uint8_t* const b = img().cdata()->cdata_b() + opts.first_pixel_offset;
        const uint8_t* const a = img().cdata()->cdata_a() + opts.first_pixel_offset;

        size_t current_pixel_idx = opts.first_pixel_offset;
        size_t skip_pattern_offset = 0;
        auto next_pixel = [&]() -> void
        {
            auto offset = opts.skip_pattern
                ? opts.skip_pattern.value()[skip_pattern_offset]
                : 0;

            current_pixel_idx += (offset + 1);
        };

        uint8_t current_channel = 0;
        auto get_channel_bit = [&](uint8_t idx) -> uint8_t
        {
            const uint8_t* chptr;
            switch(current_channel % 4)
            {
                case 0: { chptr = r; break; }
                case 1: { chptr = g; break; }
                case 2: { chptr = b; break; }
                case 3: { chptr = a; break; }
				default: { LIEN_HINT_UNREACHABLE(); } // Shush warnings
            }

            return ien::get_bit(chptr[current_pixel_idx], idx) ? 1 : 0;
        };

        auto current_pxav_ignore = [&](const pixel_availability& av) -> bool
        {
            switch(current_channel % 4)
            {
                case 0: { return av.ignore_r; }
                case 1: { return av.ignore_g; }
                case 2: { return av.ignore_b; }
                case 3: { return av.ignore_a; }
                default: { LIEN_HINT_UNREACHABLE(); } // Shush warnings
            }
        };

        auto current_pxav_bits = [&](const pixel_availability& av) -> uint8_t
        {
            switch(current_channel % 4)
            {
                case 0: { return av.bits_r; }
                case 1: { return av.bits_g; }
                case 2: { return av.bits_b; }
                case 3: { return av.bits_a; }
                default: { LIEN_HINT_UNREACHABLE(); } // Shush warnings
            }
        };

        auto next_channel = [&]() -> void
        {
            if(current_channel == 3)
            {
                current_channel = 0;
                next_pixel();
            }
            else
            {
                ++current_channel;
            }
        };
        
        uint8_t current_bit = 0;
        auto next_bit = [&]() -> uint8_t
        {
            while(true)
            {
                const pixel_availability& av = _av_map[current_pixel_idx];
                if(!current_pxav_ignore(av) && current_pxav_bits(av))
                {
                    uint8_t result = get_channel_bit(current_bit++);
                    if(current_bit == current_pxav_bits(av))
                    {
                        current_bit = 0;
                        next_channel();
                    }
                    return result;
                }
                else
                {
                    next_channel();
                }
            }
        };

        auto next_byte = [&]() -> uint8_t
        {
            uint8_t result = 0;
            for(size_t i = 0; i < 8; ++i)
            {
                uint8_t bit = next_bit();
                result |= (1 << i) * bit;
            }
            return result;
        };

        uint8_t size_header[sizeof(size_t)];
        std::memset(size_header, 0, sizeof(size_t));
        for(size_t i = 0; i < XSTEG_STEGANOGRAPHER_HEADER_SIZE; ++i)
        {
            size_header[i] = next_byte();
        }
        size_t data_len = *reinterpret_cast<size_t*>(size_header);

        ien::fixed_vector<uint8_t> result(data_len);

        for(size_t i = 0; i < data_len; ++i)
        {
            result[i] = next_byte();
        }

        return result;
    }

    ien::packed_image steganographer::gen_visual_data_image(visual_data_type type, bool inverted)
    {
        ien::packed_image result(img().width(), img().height());

        ien::fixed_vector<uint8_t> vdata(img().pixel_count(), LIEN_DEFAULT_ALIGNMENT);
        uint8_t* vdata_ptr = vdata.data();
        switch (type)
        {
            case visual_data_type::CHANNEL_RED: {
                std::memcpy(vdata_ptr, img().cdata()->cdata_r(), img().pixel_count());
                break;
            }
            case visual_data_type::CHANNEL_GREEN: {
                std::memcpy(vdata_ptr, img().cdata()->cdata_g(), img().pixel_count());
                break;
            }
            case visual_data_type::CHANNEL_BLUE: {
                std::memcpy(vdata_ptr, img().cdata()->cdata_b(), img().pixel_count());
                break;
            }
            case visual_data_type::CHANNEL_ALPHA: {
                std::memcpy(vdata_ptr, img().cdata()->cdata_a(), img().pixel_count());
                break;
            }
            case visual_data_type::AVERAGE_RGB: {
                auto result = ien::image_ops::rgb_average(img());
                std::memcpy(vdata_ptr, img().cdata()->cdata_a(), img().pixel_count());
                break;
            }
            case visual_data_type::AVERAGE_RGBA: {
                auto result = ien::image_ops::rgb_average(img());
                std::memcpy(vdata_ptr, img().cdata()->cdata_a(), img().pixel_count());
                break;
            }
            case visual_data_type::CHANNEL_SUM_SATURATED: {
                auto result = ien::image_ops::rgb_average(img());
                std::memcpy(vdata_ptr, img().cdata()->cdata_a(), img().pixel_count());
                break;
            }
            case visual_data_type::LUMINANCE: {
                ien::fixed_vector<float> vfdata = ien::image_ops::rgb_luminance(img());
                std::transform(vfdata.begin(), vfdata.end(), vdata.begin(), [](float f){ 
                    return static_cast<uint8_t>(f * 255.0F);
                });
                break;
            }
            case visual_data_type::SATURATION: {
                ien::fixed_vector<float> vfdata = ien::image_ops::rgb_luminance(img());
                std::transform(vfdata.begin(), vfdata.end(), vdata.begin(), [](float f){ 
                    return 255 - static_cast<uint8_t>(f * 255.0F);
                });
                break;
            }
        }

        for(size_t i = 0; i < vdata.size(); ++i)
        {
            uint8_t grayscale_val = inverted ? 255 - vdata[i] : vdata[i];
            uint8_t pixel[4] = { grayscale_val, grayscale_val, grayscale_val, 255 };
            std::memcpy(result.data() + (i * 4), pixel, 4);
        }
        return result;
    }

    ien::packed_image steganographer::gen_availability_map_image(const std::vector<threshold>& thresholds)
    {
        ien::packed_image result(img().width(), img().height());
        std::memset(result.data(), 0xFFu, result.pixel_count() * 4);
        uint32_t* result_ptr = reinterpret_cast<uint32_t*>(result.data());

        for(auto& th : thresholds)
        {
            auto img = gen_visual_data_image(th.type, th.inverted);
            uint32_t* data_ptr = reinterpret_cast<uint32_t*>(img.data());
            uint8_t thval = static_cast<uint8_t>(std::min(th.value * 255.0F, 255.0F));
            for(size_t i = 0; i < img.pixel_count(); ++i)
            {
                uint8_t pxval = static_cast<uint8_t>(data_ptr[i] >> 24);
                uint32_t repl_val = (pxval >= thval) ? 0xFFFFFFFFu : 0x000000FFu;
                result_ptr[i] &= repl_val;
            }
        }
        return result;
    }

    const ien::image& steganographer::img() const
    {
        return std::holds_alternative<const ien::image>(_img)
            ? std::get<const ien::image>(_img)
            : *std::get<const ien::image*>(_img);
    }
}