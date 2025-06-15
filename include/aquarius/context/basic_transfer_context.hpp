#pragma once
#include <aquarius/basic_context.hpp>

namespace aquarius
{
	namespace context
	{
		template <typename Session, typename Handler>
		class basic_transfer_context : public basic_context<Session>
		{
			using base_type = basic_context<Session>;

		public:
			basic_transfer_context(std::chrono::steady_clock::duration timeout = 30ms)
				: base_type("transfer context", timeout)
			{}

		public:
			virtual void visit(std::vector<char>& buff, std::shared_ptr<Session> session)
			{
				return handler_.visit(buff, session);
			}

		private:
			Handler handler_;
		};
	} // namespace context
} // namespace aquarius