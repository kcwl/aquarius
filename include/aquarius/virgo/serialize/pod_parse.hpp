#pragma once
#include <aquarius/detail/flex_buffer.hpp>
#include <expected>

namespace aquarius
{
	namespace virgo
	{
		struct pod_parse
		{
			template <typename T>
			bool to_datas(T&& value, detail::flex_buffer<char>& buffer)
			{
				constexpr auto size = sizeof(T);

				if (size > buffer.remain())
					return false;

				std::copy((const char*)&value, (const char*)(&value + 1), buffer.wdata());

				buffer.commit(sizeof(T));

				return true;
			}

			template <typename T>
			T from_datas(detail::flex_buffer<char>& buffer)
			{
				constexpr auto size = sizeof(T);

				if (buffer.active() < size)
					return T{};

				auto result = *(T*)buffer.rdata();

				buffer.consume(size);

				return result;
			}
		};
	} // namespace detail
} // namespace aquarius