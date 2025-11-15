#pragma once
#include <aquarius/basic_router.hpp>
#include <aquarius/virgo/http_method.hpp>
#include <aquarius/virgo/http_status.hpp>
#include <aquarius/virgo/http_fields.hpp>
#include <aquarius/virgo/http_version.hpp>
#include <aquarius/serialize/flex_buffer.hpp>
#include <aquarius/singleton.hpp>
#include <aquarius/coroutine.hpp>

namespace aquarius
{
	template <typename Session>
	class http_router
		: public basic_router<Session, virgo::http_method,
							  std::function<bool(std::shared_ptr<Session>, virgo::http_fields, flex_buffer&)>>,
		  public singleton<http_router<Session>>
	{
		using base = basic_router<Session, virgo::http_method,
								  std::function<bool(std::shared_ptr<Session>, virgo::http_fields, flex_buffer&)>>;

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
			auto func = [&](std::shared_ptr<Session> session, virgo::http_fields hf, flex_buffer& buffer)
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
								 session->get_executor(),
								 [session, req, this]() mutable -> awaitable<void>
								 {
									 auto resp = co_await std::make_shared<Context>()->visit(req);

									 send_response(session, resp);
								 },
								 detached);
						 });
				}
				catch (error_code& ec)
				{
					co_spawn(session->get_executor(),
							 [&ec, this, session]() mutable -> awaitable<void>
							 {
								 typename Context::response_t resp{};
								 resp.result(ec.value());
								 co_await send_response(session, resp);
							 });
					return false;
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

	private:
		template <typename Session, typename Response>
		auto send_response(std::shared_ptr<Session> session_ptr, Response& resp) -> awaitable<void>
		{
			flex_buffer buffer{};
			flex_buffer temp{};
			resp.commit(temp);

			auto headline =
				std::format("{} {} {}\r\n", virgo::from_string_version(resp.version()), static_cast<int>(resp.result()),
							virgo::from_status_string(resp.result()).data());

			for (auto& s : resp.fields())
			{
				headline += std::format("{}: {}\r\n", s.first, s.second);
			}

			headline += "\r\n";

			buffer.sputn(headline.c_str(), headline.size());

			buffer.sputn((char*)temp.data().data(), temp.data().size());

			co_await session_ptr->async_send(buffer);
		}
	};
} // namespace aquarius