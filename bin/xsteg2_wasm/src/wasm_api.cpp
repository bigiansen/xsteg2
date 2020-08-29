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

typedef ien::fixed_vector<uint8_t> fixed_vector_u8;

typedef std::vector<xsteg::threshold> threshold_stdvector;

#include "wasm_api.glue.cpp"