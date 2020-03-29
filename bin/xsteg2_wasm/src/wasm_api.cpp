#include <ien/image.hpp>

#include <xsteg/steganographer.hpp>
#include <stb_image.h>
#include <stdexcept>

typedef xsteg::visual_data_type visual_data_type;

struct ex_helper
{
    const char* get_exception_msg(long exptr)
    {
        return reinterpret_cast<std::exception*>(exptr)->what();
    }
};

class steganographer : private xsteg::steganographer
{
    using xs = xsteg::steganographer;

    steganographer(const ien::image& img) : xs(img) { }

    void add_threshold(const xsteg::threshold& thres, bool apply) { xs::add_threshold(thres, apply); }

    void clear_thresholds() { xs::clear_thresholds(); }

    image* encode(const uint8_t* data, size_t len, const xsteg::encoding_options& eopts)
    {
        ien::image img = xs::encode(data, len, eopts);
        return new ien::image(std::move(img));
    }
};

#include "wasm_api.glue.cpp"