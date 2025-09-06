#pragma once
#include <expected>
#include <aquarius/error_code.hpp>
#include <aquarius/detail/flex_buffer.hpp>
#include <aquarius/detail/json.hpp>

namespace aquarius
{
	namespace virgo
	{
		class null_header
		{
		public:
			template <typename T>
			std::expected<bool, error_code> commit(detail::flex_buffer<T>&)
			{
				return true;
			}

			template <typename T>
			std::expected<bool, error_code> consume(detail::flex_buffer<T>&)
			{
				return true;
			}
		};
	} // namespace virgo
} // namespace aquarius