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

		class client_invokes : public single_router<client_invokes, void, flex_buffer>
		{
			template <typename Response, typename Func>
			struct invoke_traits
			{
				static void apply(Func&& f, flex_buffer buffer)
				{
					Response resp{};
					resp.from(buffer);
					std::apply(f, resp);
				}
			};

		public:
			client_invokes() = default;

		public:
			template <typename Response, typename Func>
			void regist(std::size_t id, Func&& func)
			{
				this->map_invokes_[id] =
					std::bind(&invoke_traits<Response, Func>::apply, std::forward<Func>(func), std::placeholders::_1);
			}
		};
	} // namespace tcp
} // namespace aquarius