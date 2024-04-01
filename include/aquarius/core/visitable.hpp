#pragma once
#include <aquarius/core/visitor.hpp>

namespace aquarius
{
	template <typename _Stream, typename _Context, typename _Transfer>
	class visitable
	{
	public:
		visitable() = default;
		virtual ~visitable() = default;

	public:
		virtual bool accept(_Stream&, std::shared_ptr<_Context>, std::shared_ptr<_Transfer>) = 0;
	};

	template <typename _Request, typename _Context, typename _Transfer>
	static bool accept_shared_impl(std::shared_ptr<_Request> req, std::shared_ptr<_Context> ctx,
								   std::shared_ptr<_Transfer> session_ptr)
	{
		using visitor_t = shared_visitor<_Request, _Transfer>;

		auto visit_ptr = std::dynamic_pointer_cast<visitor_t>(ctx);

		if (!visit_ptr)
			return false;

		return visit_ptr->visit(req, session_ptr);
	}

	template <typename _Request, typename _Context, typename _Transfer>
	static bool accept_bare_impl(_Request* req, std::shared_ptr<_Context> ctx, std::shared_ptr<_Transfer> session_ptr)
	{
		using visitor_t = bare_visitor<_Request, _Transfer>;

		auto visit_ptr = std::dynamic_pointer_cast<visitor_t>(ctx);

		if (!visit_ptr)
			return false;

		return visit_ptr->visit(req, session_ptr);
	}
} // namespace aquarius