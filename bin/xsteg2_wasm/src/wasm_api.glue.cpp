
#include <emscripten.h>

extern "C" {

// Not using size_t for array indices as the values used by the javascript code are signed.
void array_bounds_check(const int array_size, const int array_idx) {
  if (array_idx < 0 || array_idx >= array_size) {
    EM_ASM({
      throw 'Array index ' + $0 + ' out of bounds: [0,' + $1 + ')';
    }, array_idx, array_size);
  }
}

// encoding_options

xsteg::encoding_options* EMSCRIPTEN_KEEPALIVE emscripten_bind_encoding_options_encoding_options_1(int offset) {
  return new xsteg::encoding_options(offset);
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_encoding_options_get_first_pixel_offset_0(xsteg::encoding_options* self) {
  return self->first_pixel_offset;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_encoding_options_set_first_pixel_offset_1(xsteg::encoding_options* self, int arg0) {
  self->first_pixel_offset = arg0;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_encoding_options___destroy___0(xsteg::encoding_options* self) {
  delete self;
}

// image

ien::image* EMSCRIPTEN_KEEPALIVE emscripten_bind_image_image_3(unsigned char* data, int w, int h) {
  return new ien::image(data, w, h);
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_image_pixel_count_0(ien::image* self) {
  return self->pixel_count();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_image___destroy___0(ien::image* self) {
  delete self;
}

// pixel_availability

xsteg::pixel_availability* EMSCRIPTEN_KEEPALIVE emscripten_bind_pixel_availability_pixel_availability_4(int r, int g, int b, int a) {
  return new xsteg::pixel_availability(r, g, b, a);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_pixel_availability___destroy___0(xsteg::pixel_availability* self) {
  delete self;
}

// ex_helper

ex_helper* EMSCRIPTEN_KEEPALIVE emscripten_bind_ex_helper_ex_helper_0() {
  return new ex_helper();
}

const char* EMSCRIPTEN_KEEPALIVE emscripten_bind_ex_helper_get_exception_msg_1(ex_helper* self, int exptr) {
  return self->get_exception_msg(exptr);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_ex_helper___destroy___0(ex_helper* self) {
  delete self;
}

// threshold

xsteg::threshold* EMSCRIPTEN_KEEPALIVE emscripten_bind_threshold_threshold_4(visual_data_type vdt, bool inv, float val, xsteg::pixel_availability* pxav) {
  return new xsteg::threshold(vdt, inv, val, *pxav);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_threshold___destroy___0(xsteg::threshold* self) {
  delete self;
}

// VoidPtr

void EMSCRIPTEN_KEEPALIVE emscripten_bind_VoidPtr___destroy___0(void** self) {
  delete self;
}

// steganographer

xsteg::steganographer* EMSCRIPTEN_KEEPALIVE emscripten_bind_steganographer_steganographer_1(ien::image* img) {
  return new xsteg::steganographer(*img);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_steganographer_add_threshold_2(xsteg::steganographer* self, xsteg::threshold* thres, bool apply) {
  self->add_threshold(*thres, apply);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_steganographer_clear_thresholds_0(xsteg::steganographer* self) {
  self->clear_thresholds();
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_steganographer_available_size_bytes_1(xsteg::steganographer* self, xsteg::encoding_options* eopts) {
  return self->available_size_bytes(*eopts);
}

ien::image* EMSCRIPTEN_KEEPALIVE emscripten_bind_steganographer_encode_3(xsteg::steganographer* self, unsigned char* data, int len, xsteg::encoding_options* eopts) {
  static ien::image temp;
  return (temp = self->encode(data, len, *eopts), &temp);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_steganographer___destroy___0(xsteg::steganographer* self) {
  delete self;
}

// visual_data_type
visual_data_type EMSCRIPTEN_KEEPALIVE emscripten_enum_visual_data_type_CHANNEL_RED() {
  return visual_data_type::CHANNEL_RED;
}
visual_data_type EMSCRIPTEN_KEEPALIVE emscripten_enum_visual_data_type_CHANNEL_GREEN() {
  return visual_data_type::CHANNEL_GREEN;
}
visual_data_type EMSCRIPTEN_KEEPALIVE emscripten_enum_visual_data_type_CHANNEL_BLUE() {
  return visual_data_type::CHANNEL_BLUE;
}
visual_data_type EMSCRIPTEN_KEEPALIVE emscripten_enum_visual_data_type_CHANNEL_ALPHA() {
  return visual_data_type::CHANNEL_ALPHA;
}
visual_data_type EMSCRIPTEN_KEEPALIVE emscripten_enum_visual_data_type_AVERAGE_RGB() {
  return visual_data_type::AVERAGE_RGB;
}
visual_data_type EMSCRIPTEN_KEEPALIVE emscripten_enum_visual_data_type_AVERAGE_RGBA() {
  return visual_data_type::AVERAGE_RGBA;
}
visual_data_type EMSCRIPTEN_KEEPALIVE emscripten_enum_visual_data_type_SATURATION() {
  return visual_data_type::SATURATION;
}
visual_data_type EMSCRIPTEN_KEEPALIVE emscripten_enum_visual_data_type_LUMINANCE() {
  return visual_data_type::LUMINANCE;
}
visual_data_type EMSCRIPTEN_KEEPALIVE emscripten_enum_visual_data_type_CHANNEL_SUM_SATURATED() {
  return visual_data_type::CHANNEL_SUM_SATURATED;
}

}

