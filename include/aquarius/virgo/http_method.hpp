#pragma once
#include <ostream>
#include <string_view>

using namespace std::string_view_literals;

namespace aquarius
{
	namespace virgo
	{
		enum class http_method
		{
			none,
			get,
			post,
			options,
			head,
			put,
			delete_,
			redirect
		};

		inline std::ostream& operator<<(std::ostream& os, http_method m)
		{
			os << static_cast<int>(m);

			return os;
		}

		inline std::string_view from_method_string(http_method m)
		{
			switch (m)
			{
			case http_method::get:
				return "GET"sv;
			case http_method::post:
				return "POST"sv;
			case http_method::options:
				return "OPTIONS"sv;
			case http_method::head:
				return "HEAD"sv;
			case http_method::put:
				return "PUT"sv;
			case http_method::delete_:
				return "DELETE"sv;
			case http_method::redirect:
				return "REDIRECT"sv;
			default:
				break;
			}

			return "UNKNOWN"sv;
		}

	} // namespace virgo
} // namespace aquarius