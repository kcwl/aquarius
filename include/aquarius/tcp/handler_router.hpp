#pragma once
#include <aquarius/basic_router.hpp>
#include <aquarius/detail/protocol.hpp>
#include <boost/asio/post.hpp>

namespace aquarius
{
	namespace tcp
	{
		template <typename Session>
		class handler_router
			: public single_router<handler_router<Session>, void, flex_buffer&, std::shared_ptr<Session>>
		{
		public:
			handler_router() = default;

		public:
			template <typename Request, typename context>
			void regist(std::size_t proto)
			{
				auto func = [&](flex_buffer& buffer, std::shared_ptr<Session> session)
				{
					auto request = std::make_shared<Request>();

					request->from_binary(buffer);

					boost::asio::post(session->get_executor(),
									  [=] { std::make_shared<context>()->visit(request, session); });
				};

				this->map_invokes_[proto] = func;
			}
		};
	} // namespace tcp
} // namespace aquarius