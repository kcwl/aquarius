#pragma once
#include <aquarius/basic_router.hpp>
#include <aquarius/ip/protocol.hpp>
#include <aquarius/serialize/flex_buffer.hpp>
#include <aquarius/virgo/header_fields.hpp>

namespace aquarius
{
	template <typename Session>
	class router
		: public basic_router<std::string_view,
							  std::function<bool(std::shared_ptr<Session>, virgo::header_fields&, flex_buffer&)>>
	{};
} // namespace aquarius