#pragma once
#include <aquarius/basic_router.hpp>
#include <aquarius/virgo/http_method.hpp>
#include <aquarius/virgo/http_status.hpp>
#include <aquarius/virgo/http_fields.hpp>

namespace aquarius
{
	template <typename Session>
	class http_router
		: public basic_router<Session, virgo::http_method,
							  std::function<bool(std::shared_ptr<Session>, virgo::http_fields, flex_buffer)>>,
		  public singleton<http_router<Session>>
	{
		using base =
			basic_router<Session, virgo::http_method, std::function<bool(std::shared_ptr<Session>, virgo::http_fields, flex_buffer)>>;

	public:
		using typename base::function_type;

		using typename base::func_trie;

	public:
		explicit http_router() = default;

		virtual ~http_router() = default;

	public:
		template <virgo::http_method Method, typename Context>
		void regist(std::string_view proto)
		{
			auto func = [&](std::shared_ptr<Session> session, virgo::http_fields hf, flex_buffer buffer)
			{
				try
				{
					auto req = std::make_shared<typename Context::request_t>();

					req->move_copy(hf);

					req->consume(buffer);

					post(session->get_executor(),
						 [=]
						 {
							 co_spawn(
								 session->get_executor(), [session, req]() mutable -> awaitable<void>
								 { co_await std::make_shared<Context>()->visit(session, req); }, detached);
						 });
				}
				catch (error_code& ec)
				{
					co_spawn(
						session->get_executor(), [ec]() mutable -> awaitable<void>
						{ ec = co_await std::make_shared<Context>()->send_response(virgo::http_status::bad_request); });
				}

				return true;
			};

			this->push(Method, proto, func);
		}
		template <typename... Args>
		bool invoke(virgo::http_method method, std::string_view key, Args&&... args)
		{
			auto iter = this->map_invokes_.find(method);
			if (iter == this->map_invokes_.end())
				return false;

			auto tree = iter->second;
			if (tree == nullptr)
				return false;

			auto func = tree->find(key);

			return func == nullptr ? false : func(std::forward<Args>(args)...);
		}
	};
} // namespace aquarius