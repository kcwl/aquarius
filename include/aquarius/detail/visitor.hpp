#pragma once
#include <aquarius/session/session.hpp>

namespace aquarius
{
	class xmessage;
}

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
			virtual _Return visit(std::shared_ptr<_Request> visited, std::shared_ptr<basic_session> session_ptr) = 0;
		};

		template <typename _Return>
		class visitable
		{
		public:
			visitable() = default;
			virtual ~visitable() = default;

		public:
			virtual _Return accept(std::shared_ptr<basic_visitor> ctx, std::shared_ptr<basic_session> session_ptr) = 0;
		};

		template <typename _Return, typename _Request>
		static _Return accept_impl(std::shared_ptr<_Request> req, std::shared_ptr<basic_visitor> ctx,
								   std::shared_ptr<basic_session> session_ptr)
		{
			using visitor_t = visitor<_Request, int>;
			using visitor_msg_t = visitor<xmessage, int>;

			if (auto visit_ptr = std::dynamic_pointer_cast<visitor_t>(ctx))
			{
				return visit_ptr->visit(req, session_ptr);
			}
			else if (auto visitor_ptr = std::dynamic_pointer_cast<visitor_msg_t>(ctx))
			{
				return visitor_ptr->visit(req, session_ptr);
			}

			return _Return{};
		}

#define DEFINE_VISITABLE(_Return)                                                                                      \
	virtual _Return accept(std::shared_ptr<aquarius::detail::basic_visitor> ctx,                                       \
						   std::shared_ptr<aquarius::basic_session> session_ptr)                                       \
	{                                                                                                                  \
		return accept_impl<_Return>(this->shared_from_this(), ctx, session_ptr);                                       \
	}

#define DEFINE_VISITABLE_NULL(_Return)                                                                                 \
	virtual _Return accept(std::shared_ptr<aquarius::detail::basic_visitor> ctx,                                       \
						   std::shared_ptr<aquarius::basic_session> session_ptr)                                       \
	{                                                                                                                  \
		return _Return{};                                                                                              \
	}

#define DEFINE_VISITOR(_Request, _Return)                                                                              \
	virtual _Return visit([[maybe_unused]]std::shared_ptr<_Request>, [[maybe_unused]] std::shared_ptr<aquarius::basic_session>)       \
	{                                                                                                                  \
		return _Return{};                                                                                              \
	}

	} // namespace detail
} // namespace aquarius