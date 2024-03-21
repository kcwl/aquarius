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
			virtual error_code accept(flex_buffer_t& buffer, std::shared_ptr<basic_context> ctx,
									  std::shared_ptr<basic_session> session_ptr, error_code& ec) = 0;
		};

		template <typename _Request>
		static error_code accept_shared_impl(std::shared_ptr<_Request> req, std::shared_ptr<basic_context> ctx,
											 std::shared_ptr<basic_session> session_ptr, error_code& ec)
		{
			using visitor_t = shared_visitor<_Request>;

			auto visit_ptr = std::dynamic_pointer_cast<visitor_t>(ctx);

			if (!visit_ptr)
				return ec = system_errc::invalid_context;

			return visit_ptr->visit(req, session_ptr, ec);
		}

		template <typename _Request>
		static error_code accept_bare_impl(_Request* req, std::shared_ptr<basic_context> ctx,
										   std::shared_ptr<basic_session> session_ptr, error_code& ec)
		{
			using visitor_t = bare_visitor<basic_message>;

			auto visit_ptr = std::dynamic_pointer_cast<visitor_t>(ctx);

			if (!visit_ptr)
				return ec = system_errc::invalid_context;

			return visit_ptr->visit(req, session_ptr, ec);
		}
	} // namespace impl
} // namespace aquarius

#define DEFINE_VISITABLE_REQUEST()                                                                                     \
	virtual aquarius::error_code accept(aquarius::flex_buffer_t& buffer, std::shared_ptr<aquarius::basic_context> ctx, \
										std::shared_ptr<aquarius::basic_session> session_ptr, aquarius::error_code& ec)     \
	{                                                                                                                  \
		this->from_binary(buffer, ec);                                                                                 \
		if (ec)                                                                                                        \
			return ec;                                                                                                 \
		return accept_shared_impl(this->shared_from_this(), ctx, session_ptr, ec);                                     \
	}

#define DEFINE_VISITABLE_RESPONSE()                                                                                    \
	virtual aquarius::error_code accept(aquarius::flex_buffer_t& buffer, std::shared_ptr<aquarius::basic_context> ctx, \
										std::shared_ptr<aquarius::basic_session> session_ptr, aquarius::error_code& ec)     \
	{                                                                                                                  \
		this->from_binary(buffer, ec);                                                                                 \
		if (ec)                                                                                                        \
			return ec;                                                                                                 \
		return accept_shared_impl(this->shared_from_this(), ctx, session_ptr, ec);                                     \
	}

#define DEFINE_VISITABLE()                                                                                             \
	virtual aquarius::error_code accept(                                                                               \
		[[maybe_unused]] aquarius::flex_buffer_t& buffer, std::shared_ptr<aquarius::basic_context> ctx,                \
		[[maybe_unused]] std::shared_ptr<aquarius::basic_session> session_ptr, aquarius::error_code& ec)                    \
	{                                                                                                                  \
		return accept_bare_impl(this, ctx, session_ptr, ec);                                                           \
	}