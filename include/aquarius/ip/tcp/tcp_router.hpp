#pragma once
#include <aquarius/basic_router.hpp>

namespace aquarius
{
	template <typename Session>
	using tcp_router = basic_router<std::string_view, std::function<bool(std::shared_ptr<Session>, flex_buffer&)>>;

} // namespace aquarius