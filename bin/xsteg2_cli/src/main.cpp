#include <ien/strutils.hpp>
#include <xsteg/availability.hpp>
#include <xsteg/steganographer.hpp>
#include <xsteg/threshold.hpp>

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "arg_iterator.hpp"

using std::optional;
using std::string;
using namespace std::literals::string_view_literals;

enum class main_mode
{
    UNSPECIFIED         = 0,
    ENCODE              = 1,
    DECODE              = 2,
    EXPORT_KEY_FILE     = 3,
    EXPORT_KEY_STRING   = 4,
    VISUAL_DATA_IMAGE   = 5,
};

struct main_args
{
    main_mode mode = main_mode::UNSPECIFIED;
    optional<string> input_image_file;
    optional<string> output_image_file;
    optional<string> input_data_file;
    optional<string> output_data_file;
    optional<string> ed_key_file;
    optional<string> ed_key_string;
    optional<string> skip_pattern;
    optional<string> first_pixel_offset;
    optional<string> visual_data_type;
    std::vector<xsteg::threshold> thresholds;
};

const std::unordered_map<std::string_view, xsteg::visual_data_type> vdt_dict = {
    { "channel_red"sv,    xsteg::visual_data_type::CHANNEL_RED },
    { "channel_green"sv,  xsteg::visual_data_type::CHANNEL_GREEN },
    { "channel_blue"sv,   xsteg::visual_data_type::CHANNEL_BLUE },
    { "channel_alpha"sv,  xsteg::visual_data_type::CHANNEL_ALPHA },
    { "red"sv,            xsteg::visual_data_type::CHANNEL_RED },
    { "green"sv,          xsteg::visual_data_type::CHANNEL_GREEN },
    { "blue"sv,           xsteg::visual_data_type::CHANNEL_BLUE },
    { "alpha"sv,          xsteg::visual_data_type::CHANNEL_ALPHA },
    { "average_rgb"sv,    xsteg::visual_data_type::AVERAGE_RGB },
    { "avg_rgb"sv,        xsteg::visual_data_type::AVERAGE_RGB },
    { "average_rgba"sv,   xsteg::visual_data_type::AVERAGE_RGBA },
    { "avg_rgba"sv,       xsteg::visual_data_type::AVERAGE_RGBA },
    { "saturation"sv,     xsteg::visual_data_type::SATURATION },
    { "sat"sv,            xsteg::visual_data_type::SATURATION },
    { "luminance"sv,      xsteg::visual_data_type::LUMINANCE },
    { "lum"sv,            xsteg::visual_data_type::LUMINANCE }
};

void parse_single_arg(arg_iterator& argit, optional<string>& target, const std::string& alias)
{
    auto arg = argit.next_arg();
    if(arg)
    {
        target = *arg;
    }
    else
    {
        throw std::invalid_argument(std::string("Missing arguments for '-") + alias + "'");
    }
}

void parse_threshold(arg_iterator& argit, main_args& margs)
{
    auto args = argit.next_args(4);
    if(args)
    {
        std::string sv_vdt = ien::strutils::to_lower((*args)[0]);
        std::string sv_inv = ien::strutils::to_lower((*args)[1]);
        std::string sv_bit = ien::strutils::to_lower((*args)[2]);
        std::string_view sv_val = (*args)[3];

        xsteg::visual_data_type vdt;
        bool inverted;
        int8_t bits[4];
        float value;

        if(vdt_dict.count(sv_vdt))
        {
            vdt = vdt_dict.at(sv_vdt);
        }
        else
        {
            throw std::invalid_argument(std::string("Invalid threshold visual data argument: ") + std::string(sv_vdt));
        }

        if(sv_inv == "i" || sv_inv == "n")
        {
            inverted = (sv_inv == "i");
        }
        else
        {
            throw std::invalid_argument(std::string("Invalid threshold inversion argument: ") + std::string(sv_inv));
        }
        
        if(sv_bit.size() == 4)
        {
            bool all_digits = std::all_of(sv_bit.begin(), sv_bit.end(), [](char ch) -> bool { 
                return static_cast<bool>(std::isdigit(ch)) || ch == 'x'; 
            });
            if(all_digits)
            {
                bits[0] = sv_bit[0] == 'x' 
                    ? -1 
                    : ien::strutils::to_integral<int8_t>(sv_bit.substr(0, 1));
                bits[1] = sv_bit[1] == 'x' 
                    ? -1 
                    : ien::strutils::to_integral<int8_t>(sv_bit.substr(1, 1));
                bits[2] = sv_bit[2] == 'x' 
                    ? -1 
                    : ien::strutils::to_integral<int8_t>(sv_bit.substr(2, 1));
                bits[3] = sv_bit[3] == 'x' 
                    ? -1 
                    : ien::strutils::to_integral<int8_t>(sv_bit.substr(3, 1));
            }
            else
            {
                throw std::invalid_argument("Invalid threshold availability bits argument: Use of non-digit or 'x' characters");
            }
        }
        else
        {
            throw std::invalid_argument("Invalid threshold availability bits argument: Invalid size");
        }
        
        bool value_digits = std::all_of(sv_val.begin(), sv_val.end(), [](char ch) -> bool {
            return std::isdigit(ch) || ch == '.';
        });
        if(value_digits)
        {
            value = ien::strutils::to_floating_point<float>(sv_val);
            if(value < 0 || value > 1)
            {
                std::cout << "Warning: Use of threshold values not between 0 and 1 may yield unexpected results" << std::endl;
            }
        }
		else
		{
			throw std::invalid_argument("Invalid threshold value argument: Invalid characters");
		}

        margs.thresholds.push_back(xsteg::threshold(
            vdt, inverted, value, {bits[0], bits[1], bits[2], bits[3]}
        ));
    }
    else
    {
        throw std::invalid_argument("Not enough arguments for threshold(-t)");
    }
}

main_args parse_args(arg_iterator& argit)
{
    main_args margs;
    while(auto arg = argit.next_arg())
    {
        std::string l_arg = ien::strutils::to_lower(*arg);
        if(l_arg == "-e" || l_arg == "--encode")
        {
            margs.mode = main_mode::ENCODE;
        }
        else if(l_arg == "-d" || l_arg == "--decode")
        {
            margs.mode = main_mode::DECODE;
        }
        else if(l_arg == "-xkf" || l_arg == "--export-key-file")
        {
            margs.mode = main_mode::EXPORT_KEY_FILE;
            parse_single_arg(argit, margs.output_data_file, "-of");
        }
        else if(l_arg == "-xks" || l_arg == "--export-ket-string")
        {
            margs.mode = main_mode::EXPORT_KEY_STRING;
        }
        else if(l_arg == "-ii" || l_arg == "--input-image")
        {
            parse_single_arg(argit, margs.input_image_file, "ii(Input Image)");
        }
        else if(l_arg == "-oi" || l_arg == "--output-image")
        {
            parse_single_arg(argit, margs.output_image_file, "oi(Output Image)");
        }
        else if(l_arg == "-if" || l_arg == "--input-file")
        {
            parse_single_arg(argit, margs.input_data_file, "if(Input data File)");
        }
        else if(l_arg == "-of" || l_arg == "--output-file")
        {
            parse_single_arg(argit, margs.output_data_file, "of(Output data File)");
        }
        else if(l_arg == "-kf" || l_arg == "--key_file")
        {
            parse_single_arg(argit, margs.ed_key_file, "kf(encode/decode Key File)");
        }
        else if(l_arg == "-ks" || l_arg == "--key-string")
        {
            parse_single_arg(argit, margs.ed_key_string, "ks(encode/decode Key String)");
        }
        else if(l_arg == "-t" || l_arg == "--threshold")
        {
            parse_threshold(argit, margs);
        }
        else if(l_arg == "-sp" || l_arg == "--skip-pattern")
        {
            parse_single_arg(argit, margs.skip_pattern, "sp(Pixel skip pattern)");
        }
        else if(l_arg == "-fp" || l_arg == "--first-pixel")
        {
            parse_single_arg(argit, margs.first_pixel_offset, "fp(First pixel)");
        }
        else if(l_arg == "-vd" || l_arg == "--gen-visual-data-image")
        {
            margs.mode = main_mode::VISUAL_DATA_IMAGE;
            parse_single_arg(argit, margs.visual_data_type, "vd(generate Visual Data image)");
        }
        else
        {
            throw std::invalid_argument("Invalid argument: " + l_arg);
        }
    }
    return margs;
}

xsteg::encoding_options extract_encoding_options(const main_args& args)
{
    xsteg::encoding_options e_opts;
    if(args.first_pixel_offset)
    {
        e_opts.first_pixel_offset = std::stoi(*args.first_pixel_offset);
    }
    if(args.skip_pattern)
    {
        std::vector<int> skip_pattern;
        for(auto segment : ien::strutils::split_view(*args.skip_pattern, '-'))
        {
            skip_pattern.push_back(ien::strutils::to_integral<int>(segment));
        }
    }

    return e_opts;
}

void encode(main_args& args)
{
    if(!args.input_image_file || !(*args.input_image_file).size())
        throw std::invalid_argument("No input file specified");
    
    if(!args.output_image_file || !(*args.output_image_file).size())
        throw std::invalid_argument("No output image file specified");
    
    if(!args.input_data_file || !(*args.input_data_file).size())
        throw std::invalid_argument("No input data file specied");
    
    if(!args.thresholds.size())
        throw std::invalid_argument("No thresholds specified");
    

    std::ifstream ifs(*args.input_data_file, std::ios::binary);
    if(!ifs)
        throw std::logic_error("Unable to open input data file!");

    std::vector<char> input_data(std::istreambuf_iterator<char>(ifs), {});

    ien::image input_image(*args.input_image_file);

    xsteg::steganographer steg(input_image);

    std::cout << "Applying thresholds..." << std::endl;
    for(size_t i = 0; i < args.thresholds.size(); ++i)
    {
        std::cout << "\r[" << i << "/" << args.thresholds.size() << "]" << std::endl;
        steg.add_threshold(args.thresholds.at(i), true);
    }
    std::cout << "\r[" << args.thresholds.size() << "/" << args.thresholds.size() << "]" << std::endl;

    std::cout << "Encoding data..." << std::endl;
    ien::image encoded_image = steg.encode(
        reinterpret_cast<uint8_t*>(input_data.data()), 
        input_data.size(), 
        extract_encoding_options(args)
    );

    std::cout << "Saving encoded image to " << *args.output_image_file << std::endl;
    encoded_image.save_to_file_png(*args.output_image_file, 9);

    std::cout << "Finished!" << std::endl;
}

void decode(main_args& args)
{
    if(!args.input_image_file || !(*args.input_image_file).size())
        throw std::invalid_argument("No input file specified");
    
    if(!args.output_data_file|| !(*args.output_data_file).size())
        throw std::invalid_argument("No output data file specified");
    
    if(!args.thresholds.size())
        throw std::invalid_argument("No thresholds specified");
    

    ien::image encoded_image(*args.input_image_file);
    xsteg::steganographer steg(encoded_image);

    std::cout << "Applying thresholds..." << std::endl;
    for(size_t i = 0; i < args.thresholds.size(); ++i)
    {
        std::cout << "\r[" << i << "/" << args.thresholds.size() << "]" << std::endl;
        steg.add_threshold(args.thresholds.at(i), true);
    }
    std::cout << "\r[" << args.thresholds.size() << "/" << args.thresholds.size() << "]" << std::endl;

    auto data = steg.decode(extract_encoding_options(args));
    std::ofstream ofs(*args.output_data_file, std::ios::binary);
    if(!ofs)
        throw std::logic_error("Could not open output data file for write");

    std::cout << "Decoding data..." << std::endl;
    ofs.write(reinterpret_cast<const char*>(data.cdata()), data.size());

    std::cout << "Finished!" << std::endl;
}

void export_key_string(main_args& args)
{
    if(args.thresholds.size() == 0)
        throw std::invalid_argument("No thresholds to generate key from!");
    
    std::cout << xsteg::gen_thresholds_key(args.thresholds);
}

void export_key_file(main_args& args)
{
    if(args.thresholds.size() == 0)
        throw std::invalid_argument("No thresholds to generate key from!");
    
    if(!args.output_data_file || !args.output_data_file.value().size())
        throw std::invalid_argument("Empty output file path!");
    
    if(std::remove((*args.output_data_file).c_str()))
        std::cout << "Overwriting key data file (" + *args.output_data_file + ")" 
                  << std::endl;
    
    std::ofstream ofs(*args.output_data_file, std::ios::out);
    if(!ofs)
        throw std::invalid_argument("Could not open file ("+ *args.output_data_file +") for writing");
    
    std::string key = xsteg::gen_thresholds_key(args.thresholds);
    ofs.write(key.data(), key.size());
}

void gen_vdata_image(main_args& args)
{
    if(!args.input_image_file || !args.input_image_file.value().size())
        throw std::invalid_argument("Empty input image path!");

    if(!args.output_image_file || !args.output_image_file.value().size())
        throw std::invalid_argument("Empty output image path!");
    
    if(!args.visual_data_type || !args.visual_data_type.value().size())
        throw std::invalid_argument("Empty visual data type argument!");
    
    if(!vdt_dict.count(*args.visual_data_type))
        throw std::invalid_argument("Invalid visual data type argument: " + *args.visual_data_type);

    ien::image img(*args.input_image_file);
    xsteg::steganographer steg(img);

    xsteg::visual_data_type vdtype = vdt_dict.at(*args.visual_data_type);
    auto result = steg.gen_visual_data_image(vdtype, false);
    result.save_to_file_png(*args.output_image_file);
}

void run_xsteg(main_args& args)
{
    switch(args.mode)
    {
        case main_mode::ENCODE: 
            encode(args); 
            break;

        case main_mode::DECODE: 
            decode(args); 
            break;

        case main_mode::EXPORT_KEY_STRING: 
            export_key_string(args); 
            break;

        case main_mode::EXPORT_KEY_FILE: 
            export_key_file(args);
            break;

        case main_mode::VISUAL_DATA_IMAGE:
            gen_vdata_image(args);
            break;

        default:
            throw std::invalid_argument("Invalid program mode!");
    }
}

int main(int argc, char* argv[])
{
    try
    {
        const string cmd(argv[0]);
        arg_iterator argit(argc, argv);
        argit.skip(1);
        auto args = parse_args(argit);
        run_xsteg(args);
        return 0;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}