
#include <emscripten.h>

extern "C" {

// Not using size_t for array indices as the values used by the javascript code are signed.

EM_JS(void, array_bounds_check_error, (size_t idx, size_t size), {
  throw 'Array index ' + idx + ' out of bounds: [0,' + size + ')';
});

void array_bounds_check(const int array_size, const int array_idx) {
  if (array_idx < 0 || array_idx >= array_size) {
    array_bounds_check_error(array_idx, array_size);
  }
}

// VoidPtr

void EMSCRIPTEN_KEEPALIVE emscripten_bind_VoidPtr___destroy___0(void** self) {
  delete self;
}

// pixel_availability

xsteg::pixel_availability* EMSCRIPTEN_KEEPALIVE emscripten_bind_pixel_availability_pixel_availability_4(int r, int g, int b, int a) {
  return new xsteg::pixel_availability(r, g, b, a);
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_pixel_availability_get_bits_r_0(xsteg::pixel_availability* self) {
  return self->bits_r;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_pixel_availability_set_bits_r_1(xsteg::pixel_availability* self, int arg0) {
  self->bits_r = arg0;
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_pixel_availability_get_bits_g_0(xsteg::pixel_availability* self) {
  return self->bits_g;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_pixel_availability_set_bits_g_1(xsteg::pixel_availability* self, int arg0) {
  self->bits_g = arg0;
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_pixel_availability_get_bits_b_0(xsteg::pixel_availability* self) {
  return self->bits_b;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_pixel_availability_set_bits_b_1(xsteg::pixel_availability* self, int arg0) {
  self->bits_b = arg0;
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_pixel_availability_get_bits_a_0(xsteg::pixel_availability* self) {
  return self->bits_a;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_pixel_availability_set_bits_a_1(xsteg::pixel_availability* self, int arg0) {
  self->bits_a = arg0;
}

bool EMSCRIPTEN_KEEPALIVE emscripten_bind_pixel_availability_get_ignore_r_0(xsteg::pixel_availability* self) {
  return self->ignore_r;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_pixel_availability_set_ignore_r_1(xsteg::pixel_availability* self, bool arg0) {
  self->ignore_r = arg0;
}

bool EMSCRIPTEN_KEEPALIVE emscripten_bind_pixel_availability_get_ignore_g_0(xsteg::pixel_availability* self) {
  return self->ignore_g;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_pixel_availability_set_ignore_g_1(xsteg::pixel_availability* self, bool arg0) {
  self->ignore_g = arg0;
}

bool EMSCRIPTEN_KEEPALIVE emscripten_bind_pixel_availability_get_ignore_b_0(xsteg::pixel_availability* self) {
  return self->ignore_b;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_pixel_availability_set_ignore_b_1(xsteg::pixel_availability* self, bool arg0) {
  self->ignore_b = arg0;
}

bool EMSCRIPTEN_KEEPALIVE emscripten_bind_pixel_availability_get_ignore_a_0(xsteg::pixel_availability* self) {
  return self->ignore_a;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_pixel_availability_set_ignore_a_1(xsteg::pixel_availability* self, bool arg0) {
  self->ignore_a = arg0;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_pixel_availability___destroy___0(xsteg::pixel_availability* self) {
  delete self;
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

// fixed_vector_u8

fixed_vector_u8* EMSCRIPTEN_KEEPALIVE emscripten_bind_fixed_vector_u8_fixed_vector_u8_1(int len) {
  return new fixed_vector_u8(len);
}

unsigned char EMSCRIPTEN_KEEPALIVE emscripten_bind_fixed_vector_u8_at_1(fixed_vector_u8* self, int index) {
  return self->at(index);
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_fixed_vector_u8_size_0(fixed_vector_u8* self) {
  return self->size();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_fixed_vector_u8___destroy___0(fixed_vector_u8* self) {
  delete self;
}

// threshold

xsteg::threshold* EMSCRIPTEN_KEEPALIVE emscripten_bind_threshold_threshold_4(visual_data_type vdt, bool inv, float val, xsteg::pixel_availability* pxav) {
  return new xsteg::threshold(vdt, inv, val, *pxav);
}

visual_data_type EMSCRIPTEN_KEEPALIVE emscripten_bind_threshold_get_type_0(xsteg::threshold* self) {
  return self->type;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_threshold_set_type_1(xsteg::threshold* self, visual_data_type arg0) {
  self->type = arg0;
}

bool EMSCRIPTEN_KEEPALIVE emscripten_bind_threshold_get_inverted_0(xsteg::threshold* self) {
  return self->inverted;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_threshold_set_inverted_1(xsteg::threshold* self, bool arg0) {
  self->inverted = arg0;
}

float EMSCRIPTEN_KEEPALIVE emscripten_bind_threshold_get_value_0(xsteg::threshold* self) {
  return self->value;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_threshold_set_value_1(xsteg::threshold* self, float arg0) {
  self->value = arg0;
}

xsteg::pixel_availability* EMSCRIPTEN_KEEPALIVE emscripten_bind_threshold_get_availability_0(xsteg::threshold* self) {
  return &self->availability;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_threshold_set_availability_1(xsteg::threshold* self, xsteg::pixel_availability* arg0) {
  self->availability = *arg0;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_threshold___destroy___0(xsteg::threshold* self) {
  delete self;
}

// image

ien::image* EMSCRIPTEN_KEEPALIVE emscripten_bind_image_image_1(const ien::image* data) {
  return new ien::image(*data);
}

ien::image* EMSCRIPTEN_KEEPALIVE emscripten_bind_image_image_3(unsigned char* data, int w, int h) {
  return new ien::image(data, w, h);
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_image_width_0(ien::image* self) {
  return self->width();
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_image_height_0(ien::image* self) {
  return self->height();
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_image_pixel_count_0(ien::image* self) {
  return self->pixel_count();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_image_save_to_file_png_2(ien::image* self, char* path, int compression_level) {
  self->save_to_file_png(path, compression_level);
}

fixed_vector_u8* EMSCRIPTEN_KEEPALIVE emscripten_bind_image_save_to_memory_png_1(ien::image* self, int compression_level) {
  static fixed_vector_u8 temp;
  return (temp = self->save_to_memory_png(compression_level), &temp);
}

std::string* EMSCRIPTEN_KEEPALIVE emscripten_bind_image_to_png_base64_1(ien::image* self, int compression_level) {
  static std::string temp;
  return (temp = self->to_png_base64(compression_level), &temp);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_image___destroy___0(ien::image* self) {
  delete self;
}

// packed_image

ien::packed_image* EMSCRIPTEN_KEEPALIVE emscripten_bind_packed_image_packed_image_0() {
  return new ien::packed_image();
}

ien::packed_image* EMSCRIPTEN_KEEPALIVE emscripten_bind_packed_image_packed_image_1(const ien::packed_image* img) {
  return new ien::packed_image(*img);
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_packed_image_width_0(ien::packed_image* self) {
  return self->width();
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_packed_image_height_0(ien::packed_image* self) {
  return self->height();
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_packed_image_pixel_count_0(ien::packed_image* self) {
  return self->pixel_count();
}

ien::image* EMSCRIPTEN_KEEPALIVE emscripten_bind_packed_image_to_image_0(ien::packed_image* self) {
  static ien::image temp;
  return (temp = self->to_image(), &temp);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_packed_image_save_to_file_png_2(ien::packed_image* self, char* path, int compression_level) {
  self->save_to_file_png(path, compression_level);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_packed_image___destroy___0(ien::packed_image* self) {
  delete self;
}

// steganographer

xsteg::steganographer* EMSCRIPTEN_KEEPALIVE emscripten_bind_steganographer_steganographer_1(const ien::image* img) {
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

ien::packed_image* EMSCRIPTEN_KEEPALIVE emscripten_bind_steganographer_gen_availability_map_image_1(xsteg::steganographer* self, const threshold_stdvector* thresholds) {
  static ien::packed_image temp;
  return (temp = self->gen_availability_map_image(*thresholds), &temp);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_steganographer___destroy___0(xsteg::steganographer* self) {
  delete self;
}

// string

std::string* EMSCRIPTEN_KEEPALIVE emscripten_bind_string_string_0() {
  return new std::string();
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_string_size_0(std::string* self) {
  return self->size();
}

char EMSCRIPTEN_KEEPALIVE emscripten_bind_string_at_1(std::string* self, int idx) {
  return self->at(idx);
}

char* EMSCRIPTEN_KEEPALIVE emscripten_bind_string_data_0(std::string* self) {
  return self->data();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_string___destroy___0(std::string* self) {
  delete self;
}

// threshold_stdvector

threshold_stdvector* EMSCRIPTEN_KEEPALIVE emscripten_bind_threshold_stdvector_threshold_stdvector_0() {
  return new threshold_stdvector();
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_threshold_stdvector_size_0(threshold_stdvector* self) {
  return self->size();
}

xsteg::threshold* EMSCRIPTEN_KEEPALIVE emscripten_bind_threshold_stdvector_at_1(threshold_stdvector* self, int idx) {
  return &self->at(idx);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_threshold_stdvector_push_back_1(threshold_stdvector* self, const xsteg::threshold* th) {
  self->push_back(*th);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_threshold_stdvector___destroy___0(threshold_stdvector* self) {
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

