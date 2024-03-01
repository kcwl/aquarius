#pragma once
#include <aquarius/elastic.hpp>
#include <aquarius/error_code.hpp>

namespace aquarius
{
	class xsession;

	class basic_message;

	class basic_context;
} // namespace aquarius

namespace aquarius
{
	namespace system
	{
		template <typename _Request>
		class basic_visitor
		{
		public:
			basic_visitor() = default;
			virtual ~basic_visitor() = default;

		public:
			virtual error_code visit(_Request visited, std::shared_ptr<xsession> session_ptr, error_code& ec) = 0;
		};

		template <typename _Request>
		class shared_visitor : public virtual basic_visitor<std::shared_ptr<_Request>>
		{
		public:
			shared_visitor() = default;
			virtual ~shared_visitor() = default;
		};

		template <typename _Request>
		class bare_visitor : public virtual basic_visitor<_Request*>
		{
		public:
			bare_visitor() = default;

			virtual ~bare_visitor() = default;
		};

		class visitable
		{
		public:
			visitable() = default;
			virtual ~visitable() = default;

		public:
			virtual error_code accept(flex_buffer_t& buffer, std::shared_ptr<basic_context> ctx,
									  std::shared_ptr<xsession> session_ptr, error_code& ec) = 0;
		};

		template <typename _Request>
		static error_code accept_shared_impl(std::shared_ptr<_Request> req, std::shared_ptr<basic_context> ctx,
										  std::shared_ptr<xsession> session_ptr, error_code& ec)
		{
			using visitor_t = shared_visitor<_Request>;

			auto visit_ptr = std::dynamic_pointer_cast<visitor_t>(ctx);

			if (!visit_ptr)
				return ec = system_errc::invalid_context;

			return visit_ptr->visit(req, session_ptr, ec);
		}

		template <typename _Request>
		static error_code accept_bare_impl(_Request* req, std::shared_ptr<basic_context> ctx,
										   std::shared_ptr<xsession> session_ptr, error_code& ec)
		{
			using visitor_t = bare_visitor<basic_message>;

			auto visit_ptr = std::dynamic_pointer_cast<visitor_t>(ctx);

			if (!visit_ptr)
				return ec = system_errc::invalid_context;

			return visit_ptr->visit(req, session_ptr, ec);
		}

#define DEFINE_VISITABLE_REQUEST()                                                                              \
	virtual aquarius::error_code accept(aquarius::flex_buffer_t& buffer, std::shared_ptr<aquarius::basic_context> ctx,              \
						   std::shared_ptr<aquarius::xsession> session_ptr, aquarius::error_code& ec)                  \
	{                                                                                                                  \
		this->from_binary(buffer, ec);                                                                                 \
		if (ec)                                                                                                        \
			return ec;                                                                           \
		return accept_shared_impl(this->shared_from_this(), ctx, session_ptr, ec);                            \
	}

#define DEFINE_VISITABLE_RESPONSE()                                                                             \
	virtual aquarius::error_code accept(aquarius::flex_buffer_t& buffer, std::shared_ptr<aquarius::basic_context> ctx,              \
						   std::shared_ptr<aquarius::xsession> session_ptr, aquarius::error_code& ec)                  \
	{                                                                                                                  \
		this->from_binary(buffer, ec);                                                                                 \
		if (ec)                                                                                                        \
			return ec;                                                                           \
		return accept_shared_impl(this->shared_from_this(), ctx, session_ptr, ec);                            \
	}

#define DEFINE_VISITABLE()                                                                                      \
	virtual aquarius::error_code accept([[maybe_unused]] aquarius::flex_buffer_t& buffer,                                           \
						   std::shared_ptr<aquarius::basic_context> ctx,                                               \
						   [[maybe_unused]] std::shared_ptr<aquarius::xsession> session_ptr, aquarius::error_code& ec) \
	{                                                                                                                  \
		return accept_bare_impl(this, ctx, session_ptr, ec);                                                  \
	}

#define DEFINE_VISITOR(_Request, _Return)                                                                              \
	virtual _Return visit([[maybe_unused]] std::shared_ptr<_Request>,                                                  \
						  [[maybe_unused]] std::shared_ptr<aquarius::xsession>)                                        \
	{                                                                                                                  \
		return _Return{};                                                                                              \
	}

	} // namespace system
} // namespace aquarius