#pragma once
#include <aquarius/serialize/binary.hpp>
#include <aquarius/virgo/basic_tcp_protocol.hpp>
#include <aquarius/virgo/tcp_header.hpp>

namespace aquarius
{
    template <detail::string_literal Router, typename Body>
    class tcp_request : public basic_tcp_protocol<true, tcp_header, Body>
    {
    public:
        using base_type = basic_tcp_protocol<true, tcp_header, Body>;

        using base_type::has_request;

        constexpr static auto this_router = detail::bind_param<Router>::value;

    public:
        tcp_request() = default;

        virtual ~tcp_request() = default;

        tcp_request(const tcp_request&) = default;

        tcp_request(tcp_request&&) noexcept = default;

        tcp_request& operator=(const tcp_request&) noexcept = default;

        tcp_request& operator=(tcp_request&&) noexcept = default;
    };
} // namespace aquarius