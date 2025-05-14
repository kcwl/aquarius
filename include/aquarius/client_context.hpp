#pragma once
#include <aquarius/basic_context.hpp>

namespace aquarius
{
	template <typename Response>
	class client_context : public basic_context<Response>
	{
		using base_type = basic_context<Response>;

	public:
		client_context(const std::string& name, const std::chrono::milliseconds& timeout = 100ms)
			: base_type(name, timeout)
		{}
	};
} // namespace aquarius