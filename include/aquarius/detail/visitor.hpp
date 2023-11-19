#pragma once
#include <aquarius/flex_buffer.hpp>

namespace aquarius
{
	class xsession;

	class xmessage;
} // namespace aquarius

namespace aquarius
{
	namespace detail
	{
		class basic_visitor
		{
		public:
			basic_visitor() = default;
			virtual ~basic_visitor() = default;
		};

		template <typename _Request, typename _Return>
		class visitor : public basic_visitor
		{
		public:
			visitor() = default;
			virtual ~visitor() = default;

		public:
			virtual _Return visit(std::shared_ptr<_Request> visited, std::shared_ptr<xsession> session_ptr) = 0;
		};

		template <typename _Return>
		class visitable
		{
		public:
			visitable() = default;
			virtual ~visitable() = default;

		public:
			virtual _Return accept(flex_buffer_t& buffer, std::shared_ptr<context> ctx,
								   std::shared_ptr<xsession> session_ptr) = 0;
		};

		template <typename _Return, typename _Request>
		static _Return accept_impl(std::shared_ptr<_Request> req, std::shared_ptr<context> ctx,
								   std::shared_ptr<xsession> session_ptr)
		{
			using visitor_t = visitor<_Request, int>;
			using visitor_msg_t = visitor<xmessage, int>;

			if (auto visit_ptr = std::dynamic_pointer_cast<visitor_t>(ctx))
			{
				return static_cast<_Return>(visit_ptr->visit(req, session_ptr));
			}
			else if (auto visitor_ptr = std::dynamic_pointer_cast<visitor_msg_t>(ctx))
			{
				return static_cast<_Return>(visitor_ptr->visit(req, session_ptr));
			}

			return _Return{};
		}

#define DEFINE_VISITABLE_REQUEST(_Return)                                                                              \
	virtual _Return accept(aquarius::flex_buffer_t& buffer, std::shared_ptr<aquarius::context> ctx,                    \
						   std::shared_ptr<aquarius::xsession> session_ptr)                                            \
	{                                                                                                                  \
		auto result = this->from_binary(buffer);                                                                       \
		if (result != aquarius::read_handle_result::ok)                                                                \
			return static_cast<_Return>(result);                                                                       \
		return accept_impl<_Return>(this->shared_from_this(), ctx, session_ptr);                                       \
	}

#define DEFINE_VISITABLE_RESPONSE(_Return)                                                                             \
	virtual _Return accept(aquarius::flex_buffer_t& buffer, std::shared_ptr<aquarius::context> ctx,                    \
						   std::shared_ptr<aquarius::xsession> session_ptr)                                            \
	{                                                                                                                  \
		auto result = this->from_binary(buffer);                                                                       \
		if (result != aquarius::read_handle_result::ok)                                                                \
			return static_cast<_Return>(result);                                                                       \
		return accept_impl<_Return>(this->shared_from_this(), ctx, session_ptr);                                       \
	}

#define DEFINE_VISITABLE_NULL(_Return)                                                                                 \
	virtual _Return accept(aquarius::flex_buffer_t& buffer, std::shared_ptr<aquarius::context> ctx,                    \
						   std::shared_ptr<aquarius::xsession> session_ptr)                                            \
	{                                                                                                                  \
		return _Return{};                                                                                              \
	}

#define DEFINE_VISITOR(_Request, _Return)                                                                              \
	virtual _Return visit([[maybe_unused]] std::shared_ptr<_Request>,                                                  \
						  [[maybe_unused]] std::shared_ptr<aquarius::xsession>)                                        \
	{                                                                                                                  \
		return _Return{};                                                                                              \
	}

	} // namespace detail
} // namespace aquarius