#include <catch2/catch.hpp>

#include <xsteg/steganographer.hpp>

void fill_image_seq(ien::image& img)
{
    uint8_t* const r = img.data()->data_r();
    uint8_t* const g = img.data()->data_g();
    uint8_t* const b = img.data()->data_b();
    uint8_t* const a = img.data()->data_a();
    for(size_t i = 0; i < img.pixel_count(); ++i)
    {
        r[i] = static_cast<uint8_t>(std::pow(i, 1));
        g[i] = static_cast<uint8_t>(std::pow(i, 2));
        b[i] = static_cast<uint8_t>(std::pow(i, 3));
        a[i] = static_cast<uint8_t>(std::pow(i, 5));
    }
}

TEST_CASE("Encode - Decode 4096x4096, 1 Threshold")
{
    ien::image img(4096, 4096);
    fill_image_seq(img);

    auto threshold = xsteg::threshold(
        xsteg::visual_data_type::SATURATION,
        false, 
        0.5F, 
        xsteg::pixel_availability(2, 2, 2, 0)
    );

    xsteg::steganographer steg(img);
    steg.add_threshold(threshold);
    steg.apply_thresholds();

    std::string data_text = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, \
    sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim \
    veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. \
    Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat \
    nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia \
    deserunt mollit anim id est laborum.";

    const uint8_t* data = reinterpret_cast<const uint8_t*>(data_text.data());
    ien::image encoded_image = steg.encode(data, data_text.size(), xsteg::encoding_options());

    xsteg::steganographer decode_steg(encoded_image);
    decode_steg.add_threshold(threshold, true);

    ien::fixed_vector<uint8_t> decoded_data = decode_steg.decode(xsteg::encoding_options());
    std::string decoded_text(reinterpret_cast<const char*>(decoded_data.cdata()), decoded_data.size());
    REQUIRE(data_text == decoded_text);
};

TEST_CASE("Encode - Decode 10 Thresholds")
{
    size_t iter = 10;
    std::vector<xsteg::threshold> thresholds;
    thresholds.reserve(iter);
    for(size_t i = 0; i < iter; ++i)
    {
        thresholds.emplace_back(
            static_cast<xsteg::visual_data_type>(rand() % 9),
            false,
            fmod((float)rand() / rand(), 1.0F),
            xsteg::pixel_availability(2, 3, 0, 1)
        );
    }

    ien::image img(2048, 2048);
    fill_image_seq(img);

    xsteg::steganographer steg(img);
    for(const auto& th : thresholds)
    {
        steg.add_threshold(th);
    }    
    steg.apply_thresholds();

    std::string data_text = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, \
    sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim \
    veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. \
    Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat \
    nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia \
    deserunt mollit anim id est laborum.";

    const uint8_t* data = reinterpret_cast<const uint8_t*>(data_text.data());
    ien::image encoded_image = steg.encode(data, data_text.size(), xsteg::encoding_options());

    xsteg::steganographer decode_steg(encoded_image);
    for(const auto& th : thresholds)
    {
		decode_steg.add_threshold(th);
    }
    decode_steg.apply_thresholds();

    ien::fixed_vector<uint8_t> decoded_data = decode_steg.decode(xsteg::encoding_options());
    std::string decoded_text(reinterpret_cast<const char*>(decoded_data.cdata()), decoded_data.size());
    REQUIRE(data_text == decoded_text);
};

TEST_CASE("Encode, 0 Thresholds, Must throw")
{
	ien::image img(4096, 4096);
	fill_image_seq(img);

	auto threshold = xsteg::threshold(
		xsteg::visual_data_type::SATURATION,
		false,
		0.5F,
		xsteg::pixel_availability(2, 2, 2, 0)
	);

	xsteg::steganographer steg(img);

	std::string data_text = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, \
    sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim \
    veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. \
    Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat \
    nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia \
    deserunt mollit anim id est laborum.";

	const uint8_t* data = reinterpret_cast<const uint8_t*>(data_text.data());
	REQUIRE_THROWS(
		steg.encode(data, data_text.size(), xsteg::encoding_options())
	);
};

TEST_CASE("Decode, 0 Thresholds, Must throw")
{
	ien::image img(4096, 4096);
	fill_image_seq(img);

	auto threshold = xsteg::threshold(
		xsteg::visual_data_type::SATURATION,
		false,
		0.5F,
		xsteg::pixel_availability(2, 2, 2, 0)
	);

	xsteg::steganographer steg(img);

	std::string data_text = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, \
    sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim \
    veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. \
    Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat \
    nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia \
    deserunt mollit anim id est laborum.";

	REQUIRE_THROWS(
		steg.decode(xsteg::encoding_options())
	);
};