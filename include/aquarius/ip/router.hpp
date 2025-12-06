#pragma once
#include <aquarius/basic_router.hpp>
#include <aquarius/ip/protocol.hpp>
#include <aquarius/virgo/http_fields.hpp>

namespace aquarius
{
	template <auto Tag, typename Session>
	class router : public basic_router<std::string_view, std::function<bool(std::shared_ptr<Session>, flex_buffer&)>>
	{};

	template <typename Session>
	class router<proto_tag::http, Session>
		: public basic_router<std::string_view,
							  std::function<bool(std::shared_ptr<Session>, virgo::http_fields, flex_buffer&)>>
	{};
} // namespace aquarius