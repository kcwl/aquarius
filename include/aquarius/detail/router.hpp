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

			using body_buffer = flex_buffer<char>;

			using function_type = std::function<bool(std::shared_ptr<Session>, body_buffer)>;

			using func_trie = detail::trie<function_type>;

		public:
			router()
				: map_invokes_(new func_trie())
			{

			}

		public:
			template <typename RPC, typename Context>
			void regist(const std::string& proto)
			{
				auto func = [&](std::shared_ptr<session> session, body_buffer&& buffer)
				{
					auto req = std::make_shared<typename RPC::request>();

					auto result = req->consume(buffer);

					if (result.has_value())
					{
						post(session->get_executor(),
							 [=]
							 {
								 co_spawn(
									 session->get_executor(),
									 [session, req] () mutable -> awaitable<void>
									 {
										 co_await std::make_shared<Context>()->visit(session, req);
									 },
									 detached);
							 });
					}

					return true;
				};
				this->map_invokes_->add(proto, func);
			}

			template<typename... Args>
			bool invoke(const std::string& key, Args... args)
			{
				auto func = this->map_invokes_->find(key);

				return func == nullptr ? false : func(args...);
			}

		private:
			std::shared_ptr<func_trie> map_invokes_;
		};
	} // namespace detail
} // namespace aquarius