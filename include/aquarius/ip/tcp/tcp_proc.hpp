#pragma once
#include <boost/asio/ip/tcp.hpp>
#include <aquarius/serialize/binary.hpp>
#include <string_view>
#include <aquarius/ip/tcp/tcp_router.hpp>
#include <aquarius/detail/string_literal.hpp>
#include <aquarius/ip/concept.hpp>

namespace aquarius
{
    template<bool Server>
    class tcp_proc
    {
        using impl_type = boost::asio::ip::tcp;

    public:
        using socket = impl_type::socket;

        using endpoint = impl_type::endpoint;

        using acceptor = impl_type::acceptor;

        using resolver = impl_type::resolver;

        using no_delay = impl_type::no_delay;

    public:
        template <typename Session>
            requires(is_session<Session>)
        auto accept(std::shared_ptr<Session> session_ptr) -> awaitable<void>
        {
            error_code ec{};

            for (;;)
            {
                flex_buffer buffer{};

                uint32_t length = 0;

                constexpr auto len = sizeof(length);

                ec = co_await session_ptr->async_read(buffer, len);

                if (ec)
                {
                    break;
                }

                buffer.sgetn((char*)&length, sizeof(uint32_t));

                ec = co_await session_ptr->async_read(buffer, length);

                if (ec)
                {
                    break;
                }

                auto router = binary_parse{}.from_datas<std::string>(buffer);

                tcp_router<Session>::get_mutable_instance().invoke(router, session_ptr, buffer);
            }

            if (ec != boost::asio::error::eof)
            {
                XLOG_ERROR() << "on read some occur error - " << ec.message();
            }

            session_ptr->shutdown();
        }
    };

    template<>
    class tcp_proc<false>
    {
        using impl_type = boost::asio::ip::tcp;

    public:
        using socket = impl_type::socket;

        using endpoint = impl_type::endpoint;

        using acceptor = impl_type::acceptor;

        using resolver = impl_type::resolver;

        using no_delay = impl_type::no_delay;

    public:
        template <typename Response, typename Session>
            requires(is_session<Session>&& is_message<Response>)
        auto query(std::shared_ptr<Session> session_ptr) -> awaitable<Response>
        {
            flex_buffer buffer{};

            uint32_t length = 0;

            constexpr auto len = sizeof(length);

            auto ec = co_await session_ptr->async_read(buffer, len);

            if (ec)
            {
                co_return Response{};
            }

            buffer.sgetn((char*)&length, len);

            ec = co_await session_ptr->async_read(buffer, length);

            if (ec)
            {
                co_return Response{};
            }

            Response resp{};

            resp.consume(buffer);

            co_return resp;
        }
    };

	template <>
	struct is_socket_type<boost::asio::ip::tcp::socket> : std::true_type
	{};
} // namespace aquarius
