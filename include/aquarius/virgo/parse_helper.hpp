#pragma once
#include <expected>
#include <string>
#include <aquarius/detail/flex_buffer.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/virgo/http_status.hpp>

namespace aquarius
{
	namespace virgo
	{
		template <typename T, char... SP>
		std::expected<std::string, error_code> read_value(detail::flex_buffer<T>& buffer)
		{
			std::string value{};
			while (!buffer.empty())
			{
				auto c = buffer.get();
				if (((c == SP) || ...))
					return value;

				value.push_back(c);
			}

			return std::unexpected(make_error_code(http_status::bad_request));
		}

		template <typename T, char... SP>
		std::expected<std::string, error_code> read_key(detail::flex_buffer<T>& buffer)
		{
			std::string value{};

			if (buffer.empty())
				return value;

			while (!buffer.empty())
			{
				auto c = buffer.get();
				if (((c == SP) || ...))
					return value;

				if (!std::isalpha(c) && c != '-')
				{
					if (c == '\r')
						return value;

					break;
				}

				value.push_back(c);
			}

			return std::unexpected(make_error_code(http_status::bad_request));
		}
	} // namespace virgo
} // namespace aquarius