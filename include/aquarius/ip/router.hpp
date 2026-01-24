#pragma once
#include <aquarius/basic_router.hpp>
#include <aquarius/ip/protocol.hpp>
#include <aquarius/virgo/header_field_base.hpp>
#include <aquarius/serialize/flex_buffer.hpp>

namespace aquarius
{
	template <typename Session>
	class router
		: public basic_router<std::string_view,
							  std::function<bool(std::shared_ptr<Session>, std::shared_ptr<header_field_base>, flex_buffer&)>>
	{};
} // namespace aquarius