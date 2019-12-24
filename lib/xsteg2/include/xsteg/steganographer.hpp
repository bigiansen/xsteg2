#pragma once

#include <xsteg/availability.hpp>
#include <xsteg/threshold.hpp>

#include <ien/fixed_vector.hpp>
#include <ien/image.hpp>

#include <optional>
#include <vector>

namespace xsteg
{
    struct encoding_options
    {
        size_t first_pixel_offset;
        std::optional<std::vector<size_t>> skip_pattern;

        constexpr encoding_options()
            : first_pixel_offset(0)
        { }

        constexpr encoding_options(size_t first_px_offs)
            : first_pixel_offset(first_px_offs)
        { }
    };

    class steganographer
    {
    private:
        const ien::img::image& _img;
        std::vector<threshold> _thresholds;
        ien::fixed_vector<pixel_availability> _av_map;

        int _last_processed_thres_idx = -1;
    
    public:
        steganographer(const ien::img::image& img);

        void add_threshold(const threshold& th, bool apply = false);
        void add_threshold(threshold&& th, bool apply = false);

        void clear_thresholds();

        bool apply_next_threshold();
        void apply_thresholds();

        size_t available_size_bytes(const encoding_options& = encoding_options()) const;

        ien::img::image write_data(const uint8_t* data, size_t len, const encoding_options& opts) const;
        ien::fixed_vector<uint8_t> read_data(const encoding_options& opts) const;
    };
}