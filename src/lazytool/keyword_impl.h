#pragma once
#include <map>
#include <string>


namespace virgo
{
    enum inner_type
    {
        error,
        _int32_t,
        _uint32_t,
        _int64_t,
        _uint64_t,
        bool_t,
        float_t,
        double_t,
        string_t,
        bytes_t,
        repeated_t,
        struct_t,
        protocol_t,
        enum_t
    };

    inline inner_type from_string(const std::string& str)
    {
        static std::map<std::string, inner_type> types = {
            {"int32_t", _int32_t},
            {"uint32_t", _uint32_t},
            {"int64_t", _int64_t},
            {"uint64_t", _uint64_t},
            {"bool", bool_t},
            {"float", float_t},
            {"double", double_t},
            {"string", string_t},
            {"bytes", bytes_t},
            {"repeated", repeated_t},
            {"struct", struct_t},
            {"protocol", protocol_t},
            {"enum", enum_t}
        };

        auto iter = types.find(str);

        if (iter == types.end())
            return inner_type::error;

        return iter->second;
    }
}