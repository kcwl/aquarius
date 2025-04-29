#pragma once
#include <aquarius/detail/flex_buffer.hpp>

namespace aquarius
{
	template <typename Request>
	class method_visitor
	{
	public:
		virtual ~method_visitor() = default;

		virtual void visit(std::unique_ptr<Request> req, flex_buffer_t& ar) = 0;
	};
} // namespace aquarius