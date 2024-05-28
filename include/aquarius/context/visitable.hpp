#pragma once
#include <aquarius/context/visitor.hpp>

namespace aquarius
{
	template <typename _Stream, typename _Context>
	class visitable
	{
	public:
		visitable() = default;
		virtual ~visitable() = default;

	public:
		virtual error_code accept(_Stream&, std::shared_ptr<_Context>, basic_connect*) = 0;
	};

	template <typename _Request, typename _Context>
	static error_code accept_shared_impl(std::shared_ptr<_Request> req, std::shared_ptr<_Context> ctx,
										 basic_connect* connect_ptr)
	{
		using visitor_t = shared_visitor<_Request>;

		auto visit_ptr = std::dynamic_pointer_cast<visitor_t>(ctx);

		if (!visit_ptr)
			return false;

		return visit_ptr->visit(req, connect_ptr);
	}

	template <typename _Request, typename _Context>
	static error_code accept_bare_impl(_Request* req, std::shared_ptr<_Context> ctx, basic_connect* connect_ptr)
	{
		using visitor_t = bare_visitor<_Request>;

		auto visit_ptr = std::dynamic_pointer_cast<visitor_t>(ctx);

		if (!visit_ptr)
			return false;

		return visit_ptr->visit(req, connect_ptr);
	}
} // namespace aquarius

#define AQUARIUS_VISITABLE_MESSAGE()                                                                                   \
	virtual aquarius::error_code accept(aquarius::flex_buffer_t& buffer, std::shared_ptr<aquarius::basic_context> ctx, \
										aquarius::basic_connect* connect_ptr)                                          \
	{                                                                                                                  \
		aquarius::error_code ec{};                                                                                     \
		ec = this->from_binary(buffer, ec);                                                                            \
		if (!ec)                                                                                                       \
			return ec;                                                                                                 \
		return accept_shared_impl(this->shared_from_this(), ctx, connect_ptr);                                         \
	}

#define AQUARIUS_VISITABLE_BASE()                                                                                      \
	virtual aquarius::error_code accept(aquarius::flex_buffer_t& buffer, std::shared_ptr<aquarius::basic_context> ctx, \
										aquarius::basic_connect* connect_ptr)                                          \
	{                                                                                                                  \
		aquarius::error_code ec{};                                                                                     \
		ec = this->from_binary(buffer, ec);                                                                            \
		if (!ec)                                                                                                       \
			return ec;                                                                                                 \
		return accept_bare_impl(this, ctx, connect_ptr);                                                               \
	}