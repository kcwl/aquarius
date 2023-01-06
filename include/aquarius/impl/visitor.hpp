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
			virtual _Return visit(_Request* visited) = 0;
		};

		template <typename _Return>
		class visitable
		{
		public:
			visitable() = default;
			virtual ~visitable() = default;

		public:
			virtual _Return accept(context* visitor) = 0;
		};

		template <typename _Request>
		static int accept_impl(_Request* req, context* v)
		{
			using visitor_t = impl::visitor<_Request, int>;
			using visitor_msg_t = impl::visitor<xmessage, int>;

			if (visitor_t* visit_ptr = dynamic_cast<visitor_t*>(v))
			{
				return visit_ptr->visit(req);
			}
			else if (visitor_msg_t* visitor_ptr = dynamic_cast<visitor_msg_t*>(v))
			{
				return visitor_ptr->visit(req);
			}

			return 0;
		}

#define DEFINE_VISITOR()                                                                                               \
	virtual int accept(aquarius::impl::context* v)                                                                     \
	{                                                                                                                  \
		return accept_impl(this, v);                                                                                   \
	}
	} // namespace impl
} // namespace aquarius