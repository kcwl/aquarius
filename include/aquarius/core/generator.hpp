#pragma once
#include <aquarius/core/router.hpp>
#include <aquarius/detail/flex_buffer.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/serialization/singleton.hpp>
#include <functional>
#include <boost/asio/post.hpp>

namespace aquarius
{
	class session_base;
}

namespace aquarius
{
	template <typename T, typename Result, typename... Args>
	class single_router : public basic_router<Result, Args...>, public boost::serialization::singleton<T>
	{};

	class context_router : public single_router<context_router, void, boost::asio::io_context&, flex_buffer_t&,
												std::shared_ptr<session_base>>
	{
	public:
		context_router() = default;

	public:
		template <typename Request, typename context>
		void regist(std::size_t proto)
		{
			auto func = [&](boost::asio::io_context& io, flex_buffer_t& buffer, std::shared_ptr<session_base> session)
			{
				auto request = std::make_shared<Request>();

				request->from_binary(buffer);

				boost::asio::post(io, [&] { std::make_shared<context>()->visit(request, session); });
			};

			map_invokes_[proto] = func;
		}
	};

} // namespace aquarius