#pragma once
#include <string>
#include <aquarius/detail/asio.hpp>
#include <aquarius/detail/flex_buffer.hpp>
#include <aquarius/error_code.hpp>
#include <functional>

namespace aquarius
{
	namespace serviced
	{
		class subscriber
		{
		public:
			using session_callback = std::function<asio::awaitable<error_code>(flex_buffer&)>;
		public:
			subscriber(std::size_t id = 0);

		public:
			std::size_t id() const;

			void id(std::size_t v);

			void attach_session(const session_callback& cb);

			auto invoke_session(flex_buffer& buffer) -> asio::awaitable<error_code>;

		private:
			std::size_t id_;

			session_callback cb_;
		};
	} // namespace serviced
} // namespace aquarius