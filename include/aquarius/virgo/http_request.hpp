#pragma once
#include <aquarius/detail/uuid_generator.hpp>
#include <aquarius/virgo/basic_http_protocol.hpp>
#include <aquarius/virgo/http_header.hpp>
#include <format>

namespace aquarius
{
    template <detail::string_literal Router, typename Body>
    class http_request : public basic_http_protocol<true, Body>
    {
    public:
        using base_type = basic_http_protocol<true, Body>;

        using base_type::has_request;

        constexpr static auto this_router = detail::bind_param<Router>::value;

    public:
        http_request() = default;

        virtual ~http_request() = default;

        http_request(const http_request&) = default;

        http_request(http_request&&) noexcept = default;

        http_request& operator=(const http_request&) = default;

        http_request& operator=(http_request&&) noexcept = default;
    };
} // namespace aquarius