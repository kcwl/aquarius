#pragma once
#include <aquarius/detail/method_visitor.hpp>

namespace aquarius
{
	class handle_method_base : public method_visitor<int>
	{
	public:
		handle_method_base() = default;

	public:
		virtual void visit(std::unique_ptr<int> req, flex_buffer_t& ar) override
		{
			return;
		}
	};
} // namespace aquarius