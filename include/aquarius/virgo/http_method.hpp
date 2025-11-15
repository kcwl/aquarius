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

		inline http_method from_string_method(std::string_view s)
		{
			if (s == "GET"sv)
				return http_method::get;
			else if (s == "POST"sv)
				return http_method::post;
			else if (s == "OPTIONS"sv)
				return http_method::options;
			else if (s == "HEAD"sv)
				return http_method::head;
			else if (s == "PUT"sv)
				return http_method::put;
			else if (s == "DELETE"sv)
				return http_method::delete_;
			else if (s == "REDIRECT"sv)
				return http_method::redirect;

			return http_method::none;
		}

	} // namespace virgo
} // namespace aquarius