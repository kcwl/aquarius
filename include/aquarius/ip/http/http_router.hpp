#pragma once
#include <aquarius/basic_router.hpp>
#include <aquarius/virgo/http_method.hpp>
#include <aquarius/virgo/http_status.hpp>
#include <aquarius/virgo/http_fields.hpp>

namespace aquarius
{

	template <typename Session>
	class http_router
		: public basic_router<Session,
							  std::function<bool(std::shared_ptr<Session>, virgo::http_fields, flex_buffer<char>)>>,
		  public singleton<http_router<Session>>
	{
		using base =
			basic_router<Session, std::function<bool(std::shared_ptr<Session>, virgo::http_fields, flex_buffer<char>)>>;

	public:
		using buffer_t = flex_buffer<char>;

		using typename base::function_type;

		using typename base::func_trie;

	public:
		explicit http_router() = default;

		virtual ~http_router() = default;

	public:
		template <typename Context>
		void regist(virgo::http_method method, std::string_view proto)
		{
			auto func = [&](std::shared_ptr<Session> session, virgo::http_fields hf, buffer_t buffer)
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

			if (method == virgo::http_method::options)
			{
				options_ptr_->add(proto, func);
			}
			else
			{
				this->map_invokes_->add(proto, func);
			}
		}
		template <typename... Args>
		bool invoke(virgo::http_method method, std::string_view key, Args... args)
		{
			function_type func;

			if (virgo::http_method::options == method)
			{
				func = options_ptr_->find(key);
			}
			else
			{
				func = this->map_invokes_->find(key);
			}

			return func == nullptr ? false : func(args...);
		}

	private:
		std::shared_ptr<func_trie> options_ptr_;
	};
} // namespace aquarius