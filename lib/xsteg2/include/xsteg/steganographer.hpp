#pragma once

#include <xsteg/availability.hpp>
#include <xsteg/threshold.hpp>

#include <ien/fixed_vector.hpp>
#include <ien/image.hpp>
#include <ien/packed_image.hpp>

#include <optional>
#include <variant>
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
        std::variant<const ien::image, const ien::image*> _img;
        std::vector<threshold> _thresholds;
        availability_map _av_map;

        int _last_processed_thres_idx = -1;
    
    public:
        steganographer(const char* filename);
        steganographer(const std::string& filename);
        steganographer(const ien::image& image);

        void add_threshold(const threshold& th, bool apply = false);
        void add_threshold(threshold&& th, bool apply = false);

        void clear_thresholds();

        bool apply_next_threshold();
        void apply_thresholds();

        void clear_availability();

        size_t available_size_bytes(const encoding_options& = encoding_options()) const;

        ien::image encode(const uint8_t* data, size_t len, const encoding_options& opts) const;
        ien::fixed_vector<uint8_t> decode(const encoding_options& opts) const;

        ien::packed_image gen_visual_data_image(visual_data_type type, bool inverted);
        ien::packed_image gen_availability_map_image(const std::vector<threshold>& thresholds);

        const ien::image& img() const;
    };
}