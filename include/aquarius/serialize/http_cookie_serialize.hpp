#pragma once
#include <aquarius/basic_serialize.hpp>
#include <aquarius/serialize/cookie.hpp>

namespace aquarius
{
	class http_cookie_serialize : public basic_serialize<cookie_parse>
	{
	public:
		http_cookie_serialize() = default;
		virtual ~http_cookie_serialize() = default;

		http_cookie_serialize(const http_cookie_serialize&) = default;
		http_cookie_serialize& operator=(const http_cookie_serialize&) = default;

		http_cookie_serialize(http_cookie_serialize&&) = default;
		http_cookie_serialize& operator=(http_cookie_serialize&&) = default;
	};
} // namespace aquarius