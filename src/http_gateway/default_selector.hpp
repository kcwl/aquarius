#pragma once
#include <aquarius/ip/protocol.hpp>
#include <string_view>

using namespace std::string_view_literals;

namespace aquarius
{
	namespace gateway
	{
		constexpr static auto __transfer__ = "transfer"sv;

		struct transfer_selector
		{
			template <typename Handler, typename Session>
			auto operator()(std::shared_ptr<Handler> handler, std::shared_ptr<Session> session_ptr)
			{
				//error_code ec{};

				//auto resp_buffer = co_await ip::mpu_transfer<T>(buffer, hf, ec);

				//if (!ec)
				//{
				//	co_await session->async_send(resp_buffer);
				//}
				//else
				//{
				//	auto buf = co_await session->make_error_response(ec);

				//	co_await session->async_send(buf);
				//}
			};
		};
	} // namespace gateway
} // namespace aquarius