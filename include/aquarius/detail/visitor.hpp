#pragma once
#include <aquarius/detail/transfer.hpp>

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
			virtual _Return visit(_Request* visited, std::shared_ptr<transfer> transfer_ptr) = 0;
		};

		template <typename _Return>
		class visitable
		{
		public:
			visitable() = default;
			virtual ~visitable() = default;

		public:
			virtual _Return accept(std::shared_ptr<basic_visitor> ctx, std::shared_ptr<transfer> transfer_ptr) = 0;
		};

		template <typename _Return, typename _Request>
		static _Return accept_impl(_Request* req, std::shared_ptr<basic_visitor> ctx,
								   std::shared_ptr<transfer> transfer_ptr)
		{
			using visitor_t = visitor<_Request, int>;
			using visitor_msg_t = visitor<xmessage, int>;

			if (auto visit_ptr = std::dynamic_pointer_cast<visitor_t>(ctx))
			{
				return visit_ptr->visit(req, transfer_ptr);
			}
			else if (auto visitor_ptr = std::dynamic_pointer_cast<visitor_msg_t>(ctx))
			{
				return visitor_ptr->visit(req, transfer_ptr);
			}

			return _Return{};
		}

#define DEFINE_VISITABLE(_Return)                                                                                      \
	virtual _Return accept(std::shared_ptr<aquarius::detail::basic_visitor> ctx,                                       \
						   std::shared_ptr<aquarius::detail::transfer> transfer_ptr)                                        \
	{                                                                                                                  \
		return accept_impl<_Return>(this, ctx, transfer_ptr);                                                              \
	}

#define DEFINE_VISITOR(_Request, _Return)                                                                              \
	virtual _Return visit(_Request*, std::shared_ptr<aquarius::detail::transfer>)                                       \
	{                                                                                                                  \
		return _Return{};                                                                                              \
	}

	} // namespace detail
} // namespace aquarius