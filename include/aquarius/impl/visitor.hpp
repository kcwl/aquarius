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

		template <typename _Return, typename _Request>
		static _Return accept_impl(_Request* req, std::shared_ptr<context> ctx)
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

			return _Return{};
		}

#define DEFINE_VISITABLE(_Return)                                                                                             \
	virtual _Return accept(std::shared_ptr<aquarius::impl::context> ctx)                                                   \
	{                                                                                                                  \
		return accept_impl<_Return>(this, ctx);                                                                                 \
	}

#define DEFINE_VISITOR(_Type, _Return)                                                                                          \
	virtual _Return visit(_Type*, visit_mode) override                                                                     \
	{                                                                                                                  \
		return 0;                                                                                                      \
	}
	} // namespace impl
} // namespace aquarius