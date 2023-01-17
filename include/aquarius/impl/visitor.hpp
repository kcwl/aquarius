#pragma once

namespace aquarius
{
	namespace impl
	{
		class xmessage;
		class context;
	} // namespace impl

	namespace impl
	{
		template <typename _Request, typename _Return>
		class visitor
		{
		public:
			visitor() = default;
			virtual ~visitor() = default;

		public:
			virtual _Return visit(_Request* visited, std::shared_ptr<session> session_ptr) = 0;
		};

		template <typename _Return>
		class visitable
		{
		public:
			visitable() = default;
			virtual ~visitable() = default;

		public:
			virtual _Return accept(std::shared_ptr<context> ctx, std::shared_ptr<session> session_ptr) = 0;
		};

		template <typename _Return, typename _Request>
		static _Return accept_impl(_Request* req, std::shared_ptr<context> ctx, std::shared_ptr<session> session_ptr)
		{
			using visitor_t = impl::visitor<_Request, int>;
			using visitor_msg_t = impl::visitor<xmessage, int>;

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
	virtual _Return accept(std::shared_ptr<aquarius::impl::context> ctx,                                               \
						   std::shared_ptr<aquarius::impl::session> session_ptr)                                       \
	{                                                                                                                  \
		return accept_impl<_Return>(this, ctx, session_ptr);                                                           \
	}

#define DEFINE_VISITOR(_Request, _Return)                                                                                        \
	virtual _Return visit(_Request*, std::shared_ptr<aquarius::impl::session>)                         \
	{                                                                                                                  \
		return _Return{};                                                                                              \
	}

	} // namespace impl
} // namespace aquarius