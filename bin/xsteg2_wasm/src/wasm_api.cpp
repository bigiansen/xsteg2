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

class steganographer
{
private:
    std::unique_ptr<xsteg::steganographer> _steg;
    
public:
    steganographer(const ien::image& img) 
        : _steg(std::make_unique<xsteg::steganographer>(img)) 
    { }

    void add_threshold(const xsteg::threshold& thres, bool apply) { _steg->add_threshold(thres, apply); }

    void clear_thresholds() { _steg->clear_thresholds(); }

    size_t available_size_bytes(const xsteg::encoding_options& eopts) { return _steg->available_size_bytes(eopts); }

    ien::image* encode(const uint8_t* data, size_t len, const xsteg::encoding_options& eopts)
    {
        ien::image img = _steg->encode(data, len, eopts);
        return new ien::image(std::move(img));
    }
};

typedef ien::fixed_vector<uint8_t> fixed_vector_u8;

typedef std::vector<xsteg::threshold> threshold_stdvector;

#include "wasm_api.glue.cpp"