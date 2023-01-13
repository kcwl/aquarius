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
			virtual _Return visit(_Request* visited, visit_mode mode = visit_mode::input) = 0;
		};

		template <typename _Return>
		class visitable
		{
		public:
			visitable() = default;
			virtual ~visitable() = default;

		public:
			virtual _Return accept(std::shared_ptr<context> ctx) = 0;
		};

		template <typename _Request>
		static int accept_impl(_Request* req, std::shared_ptr<context> ctx)
		{
			using visitor_t = impl::visitor<_Request, int>;
			using visitor_msg_t = impl::visitor<xmessage, int>;

			if (auto visit_ptr = std::dynamic_pointer_cast<visitor_t>(ctx))
			{
				return visit_ptr->visit(req);
			}
			else if (auto visitor_ptr = std::dynamic_pointer_cast<visitor_msg_t>(ctx))
			{
				return visitor_ptr->visit(req);
			}

			return 0;
		}

#define DEFINE_VISITOR()                                                                                               \
	virtual int accept(std::shared_ptr<aquarius::impl::context> ctx)                                                   \
	{                                                                                                                  \
		return accept_impl(this, ctx);                                                                                 \
	}
	} // namespace impl
} // namespace aquarius