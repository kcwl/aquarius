#pragma once
#include <aquarius/basic_router.hpp>
#include <aquarius/virgo/http_fields.hpp>
#include <aquarius/serialize/flex_buffer.hpp>

namespace aquarius
{
	template <typename Session>
	using http_router =
		basic_router<std::string_view, std::function<bool(std::shared_ptr<Session>, virgo::http_fields, flex_buffer&)>>;
} // namespace aquarius