#pragma once
#include <aquarius/basic_router.hpp>
#include <aquarius/detail/protocol.hpp>
#include <boost/asio/post.hpp>

namespace aquarius
{
	namespace tcp
	{
		template<typename Session>
		class context_router : public single_router<context_router<Session>, void, flex_buffer, std::size_t, std::shared_ptr<Session>>
		{
		public:
			context_router() = default;

		public:
			template <typename Context>
			void regist()
			{
				auto func = [&](flex_buffer buffer, std::size_t proto, std::shared_ptr<Session> session) { std::make_shared<Context>()->visit(std::move(buffer),proto, session); };

				constexpr auto mode = static_cast<std::size_t>(Context::mode);

				this->map_invokes_[mode] = func;
			}
		};

		class client_invokes : public single_router<client_invokes, void, flex_buffer>
		{
		public:
			client_invokes() = default;

		public:
			template <typename Response, typename Func>
			void regist(std::size_t id, Func&& func)
			{
				this->map_invokes_[id] = [&](flex_buffer buffer)
				{
					Response resp{};
					resp.from_binary(buffer);

					func(std::move(resp));
				};
			}
		};
	} // namespace tcp
} // namespace aquarius