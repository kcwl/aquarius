#pragma once
#include <expected>
#include <aquarius/error_code.hpp>
#include <aquarius/detail/flex_buffer.hpp>

namespace aquarius
{
	namespace virgo
	{
		class null_body
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
namespace boost
{
	namespace json
	{
		inline void tag_invoke(const boost::json::value_from_tag&, boost::json::value&, const aquarius::virgo::null_body&)
		{}

		inline aquarius::virgo::null_body tag_invoke(const boost::json::value_to_tag<aquarius::virgo::null_body>&,
											  const boost::json::value&)
		{
			return aquarius::virgo::null_body{};
		}
	} // namespace json
} // namespace boost
