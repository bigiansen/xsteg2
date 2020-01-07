#pragma once

#include <optional>
#include <string>
#include <vector>

class arg_iterator
{
private:
    std::vector<std::string_view> _args;
    size_t _current_arg = 0;

public:
    arg_iterator(int argc, char** argv);

    std::optional<std::string_view> next_arg();

    std::optional<std::vector<std::string_view>> next_args(size_t count);

    void skip(size_t count);
};