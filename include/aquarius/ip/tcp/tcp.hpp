#pragma once
#include <boost/asio/ip/tcp.hpp>
#include <aquarius/serialize/binary.hpp>
#include <string_view>
#include <aquarius/ip/tcp/tcp_router.hpp>
#include <aquarius/detail/string_literal.hpp>
#include <aquarius/ip/concept.hpp>

namespace aquarius
{

	class tcp
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
		auto accept(std::shared_ptr<Session> session_ptr) -> awaitable<error_code>
		{
			flex_buffer<char> buffer{};

			for (;;)
			{
				auto ec = co_await recv(session_ptr, buffer);

				if (!ec)
					continue;

				if (ec != boost::asio::error::eof)
				{
					XLOG_ERROR() << "on read some occur error - " << ec.what();
				}

				session_ptr->shutdown();

				co_return ec;
			}
		}

		template <typename Response, typename Session>
		requires(is_session<Session> && is_message<Response>)
		auto query(std::shared_ptr<Session> session_ptr) -> awaitable<Response>
		{
			flex_buffer<char> buffer{};

			auto ec = co_await recv_buffer(session_ptr, buffer);

			Response resp{};

			if (!ec)
			{
				resp.consume(buffer);
			}

			co_return resp;
		}

		template <typename Request, typename T>
		void make_request_buffer(std::shared_ptr<Request> request, flex_buffer<T>& buffer)
		{
			constexpr auto pos = sizeof(uint32_t);

			buffer.commit(pos);

			binary_parse parse{};
			parse.to_datas(Request::router, buffer);

			request->commit(buffer);

			auto len = static_cast<uint32_t>(buffer.tellg() - pos);

			std::copy((char*)&len, (char*)(&len + 1), buffer.data());
		}

	private:
		template <typename Session>
		auto recv(std::shared_ptr<Session> session_ptr, flex_buffer<char>& buffer) -> awaitable<error_code>
		{
			auto ec = co_await recv_buffer(session_ptr, buffer);

			if (ec)
			{
				if (ec != boost::asio::error::eof)
				{
					XLOG_ERROR() << "on read some occur error - " << ec.message();
				}
				session_ptr->shutdown();
				co_return ec;
			}

			co_spawn(
				session_ptr->get_executor(),
				[buffer = std::move(buffer), session_ptr]() mutable -> awaitable<void>
				{
					binary_parse parse{};
					auto router = parse.from_datas<std::string_view>(buffer);

					tcp_router<Session>::get_mutable_instance().invoke(router, session_ptr, buffer);

					co_return;
				},
				detached);

			co_return ec;
		}

		template <typename Session>
		auto recv_buffer(std::shared_ptr<Session> session_ptr, flex_buffer<char>& buffer) -> awaitable<error_code>
		{
			uint32_t length = 0;

			constexpr auto len = sizeof(length);

			auto ec = co_await session_ptr->async_read((char*)&length, len);

			if (ec)
			{
				if (ec != boost::asio::error::eof)
				{
					XLOG_ERROR() << "on read some occur error - " << ec.message();
				}
				session_ptr->shutdown();
				co_return ec;
			}

			co_return co_await session_ptr->async_read(buffer, length);
		}
	};
} // namespace aquarius
