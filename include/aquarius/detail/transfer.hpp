#pragma once
#include <aquarius/flex_buffer.hpp>
#include <functional>

namespace aquarius
{
	namespace detail
	{
		class transfer
		{
		public:
			transfer() = default;

			transfer(std::function<void(flex_buffer_t)>&& func)
				: conn_func_(std::move(func))

			{}

		public:
			void operator()(flex_buffer_t&& buffer)
			{
				if (!conn_func_)
					return;

				conn_func_(std::forward<flex_buffer_t>(buffer));
			}

		private:
			std::function<void(flex_buffer_t&&)> conn_func_;
		};
	} // namespace detail
} // namespace aquarius