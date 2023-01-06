#pragma once

namespace aquarius
{
	namespace proto
	{
		template<typename _Streambuf>
		class basic_message;

		using xmessage = basic_message<flex_buffer_t>;
	}

	class context;

	namespace core
	{
		template<typename _Request, typename _Return>
		class visitor
		{
		public:
			visitor() = default;
			virtual ~visitor() = default;

		public:
			virtual _Return visit(_Request* visited) = 0;
		};

		template<typename _Return>
		class visitable
		{
		public:
			visitable() = default;
			virtual ~visitable() = default;

		public:
			virtual _Return accept(aquarius::context* visitor) = 0;

		protected:
			template<typename _Request>
			static _Return accept_impl(_Request* req, aquarius::context* v)
			{
				using visitor_t = visitor<_Request, _Return>;
				using msg_visitor_t = visitor<proto::xmessage, _Return>;

				if (auto visit_ptr = dynamic_cast<visitor_t*>(v))
				{
					return visit_ptr->visit(req);
				}
				else if (auto visit_msg_ptr = dynamic_cast<msg_visitor_t*>(v))
				{
					return visit_msg_ptr->visit(req);
				}

				return _Return{};
			}
		};
	}
}