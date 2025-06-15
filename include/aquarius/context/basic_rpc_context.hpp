#pragma once
#include <aquarius/basic_context.hpp>
#include <aquarius/context/detail/client_router.hpp>

namespace aquarius
{
	namespace context
	{
		template <typename Session>
		class basic_rpc_context : public basic_context<Session>
		{
			using base_type = basic_context<Session>;

			using buffer_type = typename Session::buffer_type;

		public:
			basic_rpc_context(std::chrono::steady_clock::duration timeout = 30ms)
				: base_type("rpc context", timeout)
			{}

		public:
			template <typename Response, typename Func>
			void regist(std::size_t context_id, Func&& f)
			{
				detail::client_router::get_mutable_instance().template regist<Response>(context_id,
																						std::forward<Func>(f));
			}

			virtual void visit(buffer_type buff, std::shared_ptr<Session> session) override
			{
				post(session->get_executor(),
					 [session, buff = std::move(buff), this] mutable
					 {
						 this->invoke(session,
									  [session, buff = std::move(buff)]
									  {
										  uint32_t proto{};
										  buff.load((uint8_t*)&proto, sizeof(proto));

										  detail::client_router::get_mutable_instance().invoke(proto, std::move(buff));
									  });
					 });
			}
		};
	} // namespace context
} // namespace aquarius