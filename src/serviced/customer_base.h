#pragma once
#include <aquarius.hpp>

namespace aquarius
{
	namespace serviced
	{
		class customer_base
		{
			using func_t =
				std::function<asio::awaitable<std::expected<flex_buffer, error_code>>(flex_buffer&, int32_t)>;

		public:
			customer_base() = default;
			virtual ~customer_base() = default;

		public:
			virtual std::string group() const = 0;

			virtual auto heart() -> asio::awaitable<void> = 0;

			virtual func_t subscribe() = 0;

			virtual int32_t weight() const = 0;
		};
	} // namespace serviced
} // namespace aquarius