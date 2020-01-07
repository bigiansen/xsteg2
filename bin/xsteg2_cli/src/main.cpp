#include <xsteg/availability.hpp>

#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include "arg_iterator.hpp"

using std::optional;
using std::string;

enum class main_mode
{
    UNSPECIFIED = 0,
    ENCODE      = 1,
    DECODE      = 2
};

struct main_args
{
    main_mode mode;
    optional<string> input_image_file;
    optional<string> output_image_file;
    optional<string> input_data_file;
    optional<string> output_data_file;
    optional<string> ed_key_file;
    optional<string> ed_key_string;
    std::vector<xsteg::pixel_availability> thresholds;
};

int main(int argc, char* argv[])
{
    const string cmd(argv[0]);
    
    arg_iterator argit(argc, argv);
    argit.skip(1);

    // ...

    return 0;
}