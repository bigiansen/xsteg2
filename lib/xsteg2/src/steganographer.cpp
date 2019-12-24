#include <xsteg/steganographer.hpp>

#include <ien/assert.hpp>
#include <ien/bit_tools.hpp>
#include <ien/multi_array_bit_iterator.hpp>

#ifdef XSTEG_STEGANOGRAPHER_HEADER_SIZE
    #warn Note that using different header sizes WILL break compatibility between different compilations
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
    steganographer::steganographer(const ien::img::image& img)
        : _img(img)
        , _av_map(img.pixel_count())
    { 
        std::fill(_av_map.begin(), _av_map.end(), pixel_availability(-1, -1, -1, -1));
    }

    void steganographer::add_threshold(const threshold& th, bool apply)
    {
        _thresholds.push_back(th);
    }

    void steganographer::add_threshold(threshold&& th, bool apply)
    {
        _thresholds.push_back(std::move(th));
    }

    void steganographer::clear_thresholds()
    {
        _thresholds.clear();
        _last_processed_thres_idx = -1;
    }

    bool steganographer::apply_next_threshold()
    {
        if(++_last_processed_thres_idx >= _thresholds.size()) 
        { return false; }

        threshold& th = _thresholds.at(_last_processed_thres_idx);
        apply_threshold(_av_map, _img, th);
        return true;
    }

    void steganographer::apply_thresholds()
    {
        while(apply_next_threshold()) { continue; }
    }

    size_t steganographer::available_size_bytes(const encoding_options& opts) const
    {
        size_t bits = 0;
        if(opts.skip_pattern)
        {
            size_t pattern_idx = 0;
            for(size_t i = opts.first_pixel_offset; 
                i < _img.pixel_count(); 
                i += (opts.skip_pattern.value()[pattern_idx++]))
            {
                const auto& av = _av_map[i];
                bits += (av.bits_r + av.bits_g + av.bits_b + av.bits_a);
            }
        }
        else
        {
            for(size_t i = opts.first_pixel_offset; i < _img.pixel_count(); ++i)
            {
                const auto& av = _av_map[i];
                bits += (av.bits_r + av.bits_g + av.bits_b + av.bits_a);
            }
        }
		size_t result = bits / 8;
		return result < XSTEG_STEGANOGRAPHER_HEADER_SIZE
			? 0 
			: result - XSTEG_STEGANOGRAPHER_HEADER_SIZE;

        return result;
    }
    
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ ATTENTION +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // The following code is an absolute mess.
    // There is just too much state floating around to cleanly separate it,
    // so i've kept it separated into lambdas.
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

    // Visual aid
    #define LAMBDA auto

    ien::img::image steganographer::write_data(const uint8_t* data, size_t len, const encoding_options& opts) const
    {
        ien::runtime_assert(
            len > 0,
            "Attempt to encode 0-length data!"
        );

        ien::runtime_assert(
            len <= available_size_bytes(opts),
            "Not enough space available to encode data!  Requested: " +
                std::to_string(len) + " Available: " + std::to_string(available_size_bytes())
        );

        ien::img::image result = _img;

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
        LAMBDA next_pixel = [&]() -> void
        {
            auto offset = opts.skip_pattern
                ? opts.skip_pattern.value()[skip_pattern_offset]
                : 0;
                
            current_pixel_idx += (offset + 1);
        };

        size_t processed_bits = 0;
        LAMBDA next_bits_mask = [&](uint8_t amount) -> uint8_t
        {
            ien::debug_assert(amount <= 8, "Requested too many bits!");
            uint8_t result = 0;
            for(size_t i = 0; i < amount; ++i)
            {
                result |= ((1 * (static_cast<uint8_t>(*data_it)) << i));
                ++data_it;
            }
            return result;
        };

        LAMBDA write_bits_channel = [&](uint8_t* ch, bool ignore, uint8_t bits) -> bool
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

    ien::fixed_vector<uint8_t> steganographer::read_data(const encoding_options& opts) const
    {
		ien::runtime_assert(
			available_size_bytes(opts) > XSTEG_STEGANOGRAPHER_HEADER_SIZE,
			"Specified thresholds cannot hold any data!"
		);
        const uint8_t* const r = _img.cdata()->cdata_r() + opts.first_pixel_offset;
        const uint8_t* const g = _img.cdata()->cdata_g() + opts.first_pixel_offset;
        const uint8_t* const b = _img.cdata()->cdata_b() + opts.first_pixel_offset;
        const uint8_t* const a = _img.cdata()->cdata_a() + opts.first_pixel_offset;

        size_t current_pixel_idx = opts.first_pixel_offset;
        size_t skip_pattern_offset = 0;
        LAMBDA next_pixel = [&]() -> void
        {
            auto offset = opts.skip_pattern
                ? opts.skip_pattern.value()[skip_pattern_offset]
                : 0;

            current_pixel_idx += (offset + 1);
        };

        uint8_t current_channel = 0;
        LAMBDA get_channel_bit = [&](uint8_t idx) -> uint8_t
        {
            const uint8_t* chptr;
            switch(current_channel % 4)
            {
                case 0: { chptr = r; break; }
                case 1: { chptr = g; break; }
                case 2: { chptr = b; break; }
                case 3: { chptr = a; break; }
            }

            return ien::get_bit(chptr[current_pixel_idx], idx) ? 1 : 0;
        };

        LAMBDA current_pxav_ignore = [&](const pixel_availability& av) -> bool
        {
            switch(current_channel % 4)
            {
                case 0: { return av.ignore_r; }
                case 1: { return av.ignore_g; }
                case 2: { return av.ignore_b; }
                case 3: { return av.ignore_a; }
                default:
                    throw std::logic_error("??????????");
            }
        };

        LAMBDA current_pxav_bits = [&](const pixel_availability& av) -> uint8_t
        {
            switch(current_channel % 4)
            {
                case 0: { return av.bits_r; }
                case 1: { return av.bits_g; }
                case 2: { return av.bits_b; }
                case 3: { return av.bits_a; }
                default:
                    throw std::logic_error("??????????");
            }
        };

        LAMBDA next_channel = [&]() -> void
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
        LAMBDA next_bit = [&]() -> uint8_t
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

        LAMBDA next_byte = [&]() -> uint8_t
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
}