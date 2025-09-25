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
		std::string read_value(detail::flex_buffer<T>& buffer, error_code& ec)
		{
			std::string value{};
			while (!buffer.empty())
			{
				auto c = buffer.get();
				if (((c == SP) || ...))
					return value;

				if (c == ' ')
					continue;

				value.push_back(c);
			}

			ec = make_error_code(http_status::bad_request);

			return value;
		}

		template <typename T, char... SP>
		std::string read_key(detail::flex_buffer<T>& buffer, error_code& ec)
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

			ec = make_error_code(http_status::bad_request);

			return {};
		}
	} // namespace virgo
} // namespace aquarius