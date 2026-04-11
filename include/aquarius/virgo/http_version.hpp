#pragma once
#include <aquarius/virgo/http_status.hpp>
#include <expected>
#include <ostream>
#include <string_view>

using namespace std::string_view_literals;

namespace aquarius
{
	enum class http_version
	{
		http1_0,
		http1_1
	};

	inline static std::map<http_version, std::string_view> http_versions = { { http_version::http1_0, "HTTP/1.0"sv },
																			 { http_version::http1_1, "HTTP/1.1"sv } };

	inline std::string_view version_to_string(http_version v)
	{
		auto iter = http_versions.find(v);

		return iter != http_versions.end() ? iter->second : "Unknown"sv;
	}

	inline auto string_to_version(std::string_view s) -> std::expected<http_version, error_code>
	{
		auto iter = std::find_if(http_versions.begin(), http_versions.end(), [&](auto v) { return v.second == s; });

		if (iter == http_versions.end())
		{
			return std::unexpected(http_status::http_version_not_supported);
		}

		return iter->first;
	}

	inline std::ostream& operator<<(std::ostream& os, http_version v)
	{
		os << version_to_string(v);

		return os;
	}

	inline std::istream& operator>>(std::istream& is, http_version& v)
	{
		std::string result{};
		is >> result;

		[[maybe_unused]] auto res =
			string_to_version(result).and_then([&](auto ver) -> std::expected<void, error_code> { v = ver; });

		return is;
	}
} // namespace aquarius