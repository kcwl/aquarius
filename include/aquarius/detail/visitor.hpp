#pragma once
#include <aquarius/elastic.hpp>

namespace aquarius
{
	class xsession;

	class basic_message;

	class basic_context;
} // namespace aquarius

namespace aquarius
{
	namespace detail
	{
		template <typename _Request, typename _Return>
		class basic_visitor
		{
		public:
			basic_visitor() = default;
			virtual ~basic_visitor() = default;

		public:
			virtual _Return visit(_Request visited, std::shared_ptr<xsession> session_ptr) = 0;
		};

		template <typename _Request, typename _Return>
		class shared_visitor : public virtual basic_visitor<std::shared_ptr<_Request>, _Return>
		{
		public:
			shared_visitor() = default;
			virtual ~shared_visitor() = default;
		};

		template <typename _Request, typename _Return>
		class bare_visitor : public virtual basic_visitor<_Request*, _Return>
		{
		public:
			bare_visitor() = default;

			virtual ~bare_visitor() = default;
		};

		template <typename _Return>
		class visitable
		{
		public:
			visitable() = default;
			virtual ~visitable() = default;

		public:
			virtual _Return accept(flex_buffer_t& buffer, std::shared_ptr<basic_context> ctx,
								   std::shared_ptr<xsession> session_ptr) = 0;
		};

		template <typename _Return, typename _Request>
		static _Return accept_shared_impl(std::shared_ptr<_Request> req, std::shared_ptr<basic_context> ctx,
			std::shared_ptr<xsession> session_ptr)
		{
			using visitor_t = shared_visitor<_Request, int>;

			auto visit_ptr = std::dynamic_pointer_cast<visitor_t>(ctx);

			if (!visit_ptr)
				return _Return{};

			return static_cast<_Return>(visit_ptr->visit(req, session_ptr));
		}

		template <typename _Return, typename _Request>
		static _Return accept_bare_impl(_Request* req, std::shared_ptr<basic_context> ctx,
			std::shared_ptr<xsession> session_ptr)
		{
			using visitor_t = bare_visitor<basic_message, int>;

			auto visit_ptr = std::dynamic_pointer_cast<visitor_t>(ctx);

			if (!visit_ptr)
				return _Return{};

			return static_cast<_Return>(visit_ptr->visit(req, session_ptr));
		}

#define DEFINE_VISITABLE_REQUEST(_Return)                                                                              \
	virtual _Return accept(aquarius::flex_buffer_t& buffer, std::shared_ptr<aquarius::basic_context> ctx,              \
						   std::shared_ptr<aquarius::xsession> session_ptr)                                            \
	{                                                                                                                  \
		auto result = this->from_binary(buffer);                                                                       \
		if (result != aquarius::read_handle_result::ok)                                                                \
			return static_cast<_Return>(result);                                                                       \
		return accept_shared_impl<_Return>(this->shared_from_this(), ctx, session_ptr);                                       \
	}

#define DEFINE_VISITABLE_RESPONSE(_Return)                                                                             \
	virtual _Return accept(aquarius::flex_buffer_t& buffer, std::shared_ptr<aquarius::basic_context> ctx,              \
						   std::shared_ptr<aquarius::xsession> session_ptr)                                            \
	{                                                                                                                  \
		auto result = this->from_binary(buffer);                                                                       \
		if (result != aquarius::read_handle_result::ok)                                                                \
			return static_cast<_Return>(result);                                                                       \
		return accept_shared_impl<_Return>(this->shared_from_this(), ctx, session_ptr);                                       \
	}

#define DEFINE_VISITABLE(_Return)                                                                                      \
	virtual _Return accept([[maybe_unused]] aquarius::flex_buffer_t& buffer,                                           \
						   std::shared_ptr<aquarius::basic_context> ctx,                                               \
						   [[maybe_unused]] std::shared_ptr<aquarius::xsession> session_ptr)                           \
	{                                                                                                                  \
		return accept_bare_impl<_Return>(this, ctx, session_ptr);  \
	}

#define DEFINE_VISITOR(_Request, _Return)                                                                              \
	virtual _Return visit([[maybe_unused]] std::shared_ptr<_Request>,                                                  \
						  [[maybe_unused]] std::shared_ptr<aquarius::xsession>)                                        \
	{                                                                                                                  \
		return _Return{};                                                                                              \
	}

	} // namespace detail
} // namespace aquarius