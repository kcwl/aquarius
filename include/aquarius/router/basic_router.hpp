#pragma once
#include <aquarius/detail/trie.hpp>
#include <memory>
#include <functional>
#include <span>
#include <aquarius/router/type.hpp>

namespace aquarius
{
	template <typename Session>
	class basic_router
	{
	public:
		constexpr static router_type type = router_type::basic_router;

		using body_buffer = std::span<char>;

		using function_type = std::function<bool(std::shared_ptr<Session>, body_buffer, typename Session::header)>;

		using func_trie = detail::trie<function_type>;

	public:
		basic_router()
			: map_invokes_(new func_trie())
		{}

		virtual ~basic_router() = default;

	public:
		template <typename RPC, typename Context>
		void regist(std::string_view proto)
		{
			auto func = [&](std::shared_ptr<Session> session, body_buffer&& buffer, typename Session::header h)
			{
				auto req = std::make_shared<typename RPC::request>(h);
				req->consume(buffer);
				post(session->get_executor(),
					 [=]
					 {
						 co_spawn(
							 session->get_executor(), [session, req]() mutable -> awaitable<void>
							 { co_await std::make_shared<Context>()->visit(session, req); }, detached);
					 });
				return true;
			};
			this->map_invokes_->add(proto, func);
		}

		template <typename... Args>
		bool invoke(std::string_view key, Args... args)
		{
			auto func = this->map_invokes_->find(key);

			return func == nullptr ? false : func(args...);
		}

	private:
		std::shared_ptr<func_trie> map_invokes_;
	};
} // namespace aquarius