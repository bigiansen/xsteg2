#include "arg_iterator.hpp"

arg_iterator::arg_iterator(int argc, char** argv)
{
    _args.reserve(argc);
    for(int i = 0; i < argc; ++i)
    {
        _args.push_back(std::string_view(argv[i]));
    }
}

std::optional<std::string_view> arg_iterator::next_arg()
{
    return (_current_arg < _args.size())
        ? std::optional<std::string_view>(_args[_current_arg++])
        : std::nullopt;
}

std::optional<std::vector<std::string_view>> arg_iterator::next_args(size_t count)
{
    if(_current_arg + count > _args.size())
    {
        return std::nullopt;
    }
    else
    {
        std::vector<std::string_view> result;
        result.reserve(count);
        for(size_t i = 0; i < count; ++i)
        {
            result.push_back(_args[_current_arg++]);
        }
        return std::optional<std::vector<std::string_view>>(std::move(result));
    }
}

void arg_iterator::skip(size_t count)
{
    _current_arg += count;
}