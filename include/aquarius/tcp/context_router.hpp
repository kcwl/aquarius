#pragma once
#include <aquarius/basic_router.hpp>
#include <aquarius/detail/flex_buffer.hpp>
#include <boost/asio/post.hpp>

namespace aquarius
{

	template <typename Session>
	class context_router : public single_router<context_router<Session>, void, flex_buffer_t&, std::shared_ptr<Session>>
	{
	public:
		context_router() = default;

	public:
		template <typename Request, typename context>
		void regist(std::size_t proto)
		{
			auto func = [&](flex_buffer_t& buffer, std::shared_ptr<Session> session)
			{
				auto request = std::make_shared<Request>();

				request->from_binary(buffer);

				boost::asio::post(session->get_executor(),
								  [=] { std::make_shared<context>()->visit(request, session); });
			};

			this->map_invokes_[proto] = func;
		}
	};

} // namespace aquarius