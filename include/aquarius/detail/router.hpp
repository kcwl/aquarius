#pragma once
#include <aquarius/awaitable.hpp>
#include <aquarius/detached.hpp>
#include <aquarius/detail/trie.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/post.hpp>
#include <aquarius/singleton.hpp>
#include <type_traits>
#include <vector>

namespace aquarius
{
	namespace detail
	{
		template <typename Session>
		class router : public singleton<router<Session>>
		{
		public:
			using session = Session;

			using header = typename Session::header;

			using body_buffer = std::vector<char>;

			using function_type = std::function<bool(std::shared_ptr<Session>, body_buffer, typename Session::header)>;

			using func_trie = detail::trie<function_type>;

		public:
			router() = default;

		public:
			template <typename RPC, typename Context>
			void regist(std::string_view proto)
			{
				auto func = [&](std::shared_ptr<session> session, body_buffer&& buffer, header h)
				{
					auto req = std::make_shared<typename RPC::request>(h);
					req->consume(buffer);
					post(session->get_executor(),
						 [=]
						 {
							 co_spawn(
								 session->get_executor(),
								 [session, req]() mutable -> awaitable<void>
								 {
									 co_await std::make_shared<Context>()->visit(session, req);
								 },
								 detached);
						 });
					return true;
				};
				this->map_invokes_->add(proto, func);
			}

			template<typename... Args>
			bool invoke(std::string_view key, Args... args)
			{
				auto func = this->map_invokes_->find(key);

				return func == nullptr ? false : func(args...);
			}

		private:
			std::shared_ptr<func_trie> map_invokes_;
		};
	} // namespace detail
} // namespace aquarius