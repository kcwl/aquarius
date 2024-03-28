#pragma once
#include <aquarius/context/impl/visitor.hpp>
#include <aquarius/core/elastic.hpp>
#include <aquarius/core/error_code.hpp>

namespace aquarius
{
	class basic_message;

	class basic_session;

	class basic_context;
} // namespace aquarius

namespace aquarius
{
	namespace impl
	{
		class visitable
		{
		public:
			visitable() = default;
			virtual ~visitable() = default;

		public:
			virtual bool accept(flex_buffer_t& buffer, std::shared_ptr<basic_context> ctx,
								std::shared_ptr<basic_session> session_ptr) = 0;
		};

		template <typename _Request>
		static bool accept_shared_impl(std::shared_ptr<_Request> req, std::shared_ptr<basic_context> ctx,
									   std::shared_ptr<basic_session> session_ptr)
		{
			using visitor_t = shared_visitor<_Request>;

			auto visit_ptr = std::dynamic_pointer_cast<visitor_t>(ctx);

			if (!visit_ptr)
				return false;

			return visit_ptr->visit(req, session_ptr);
		}

		template <typename _Request>
		static bool accept_bare_impl(_Request* req, std::shared_ptr<basic_context> ctx,
									 std::shared_ptr<basic_session> session_ptr)
		{
			using visitor_t = bare_visitor<basic_message>;

			auto visit_ptr = std::dynamic_pointer_cast<visitor_t>(ctx);

			if (!visit_ptr)
				return false;

			return visit_ptr->visit(req, session_ptr);
		}
	} // namespace impl
} // namespace aquarius

#define DEFINE_VISITABLE_REQUEST()                                                                                     \
	virtual bool accept(aquarius::flex_buffer_t& buffer, std::shared_ptr<aquarius::basic_context> ctx,                 \
						std::shared_ptr<aquarius::basic_session> session_ptr)                                          \
	{                                                                                                                  \
		aquarius::error_code ec{};                                                                                     \
		this->from_binary(buffer, ec);                                                                                 \
		if (ec)                                                                                                        \
			return ec;                                                                                                 \
		return accept_shared_impl(this->shared_from_this(), ctx, session_ptr);                                         \
	}

#define DEFINE_VISITABLE_RESPONSE()                                                                                    \
	virtual bool accept(aquarius::flex_buffer_t& buffer, std::shared_ptr<aquarius::basic_context> ctx,                 \
						std::shared_ptr<aquarius::basic_session> session_ptr)                                          \
	{                                                                                                                  \
		aquarius::error_code ec{};                                                                                     \
		this->from_binary(buffer, ec);                                                                                 \
		if (ec)                                                                                                        \
			return ec;                                                                                                 \
		return accept_shared_impl(this->shared_from_this(), ctx, session_ptr);                                         \
	}

#define DEFINE_VISITABLE()                                                                                             \
	virtual bool accept(aquarius::flex_buffer_t& buffer, std::shared_ptr<aquarius::basic_context> ctx,                 \
						std::shared_ptr<aquarius::basic_session> session_ptr)                                          \
	{                                                                                                                  \
		aquarius::error_code ec{};                                                                                     \
		this->from_binary(buffer, ec);                                                                                 \
		if (ec)                                                                                                        \
			return ec;                                                                                                 \
		return accept_bare_impl(this, ctx, session_ptr);                                                               \
	}