#pragma once
#include <aquarius/virgo/http_status.hpp>
#include <expected>
#include <map>
#include <ostream>
#include <string_view>

using namespace std::string_view_literals;

namespace aquarius
{
	enum class http_method
	{
		post,
		get,
		options,
		head,
		put,
		delete_
	};

	inline static std::map<http_method, std::string_view> http_methods = {
		{ http_method::get, "GET"sv },	 { http_method::post, "POST"sv }, { http_method::options, "OPTIONS"sv },
		{ http_method::head, "HEAD"sv }, { http_method::put, "PUT"sv },	  { http_method::delete_, "DELETE"sv }
	};

	inline std::string_view method_to_string(http_method m)
	{
		auto iter = http_methods.find(m);

		return iter != http_methods.end() ? iter->second : "UNKNOWN"sv;
	}

	inline auto string_to_method(std::string_view s) -> std::expected<http_method, error_code>
	{
		auto iter = std::find_if(http_methods.begin(), http_methods.end(), [&](auto m) { return m.second == s; });

		if (iter == http_methods.end())
		{
			return std::unexpected(http_status::method_not_allowed);
		}

		auto dist = std::distance(http_methods.begin(), iter);

		return static_cast<http_method>(dist);
	}

	inline std::ostream& operator<<(std::ostream& os, http_method m)
	{
		os << method_to_string(m);

		return os;
	}

	inline std::istream& operator>>(std::istream& is, http_method& m)
	{
		std::string method{};

		is >> method;

		[[maybe_unused]] auto res =
			string_to_method(method).and_then([&] (auto method) -> std::expected<bool, error_code> { m = method; return true; });

		return is;
	}
} // namespace aquarius