#pragma once
#include <aquarius/ip/handler.hpp>
#include <aquarius/virgo/http_method.hpp>

namespace aquarius
{
	template <auto Method, typename Session, typename Request, typename Response>
	class http_method_handler;

	template <typename Session, typename Request, typename Response>
	class http_method_handler<virgo::http_method::options, Session, Request, Response>
		: public handler<proto::http, Session, Request, Response>
	{
	public:
		http_method_handler()
			: basic_handler<Session, Request, Response>("__http_options_handler")
		{}

	public:
		virtual auto handler() -> awaitable<error_code> override
		{
			response().version(virgo::http_version::http1_1);
			response().set_field("Access-Control-Allow-Origin", co_await mpc_http_origin());
			response().set_field("Access-Control-Request-Method", "POST");
			response().set_field("Access-Control-Allow-Headers", "content-type");

			co_return virgo::http_status::ok;
		}
	};

	template <typename Session, typename Request, typename Response>
	class http_method_handler<virgo::http_method::get, Session, Request, Response>
		: public handler<proto::http, Session, Request, Response>
	{
	public:
		http_method_handler()
			: basic_handler<Session, Request, Response>("__http_get_handler")
		{}

	public:
		virtual auto handler() -> awaitable<error_code> override
		{
			auto router = request().url_path();

			auto file_path = (co_await mpc_http_root()) + std::string(router.data(), router.size());

			std::fstream ifs(file_path, std::ios::in | std::ios::binary);
			if (!ifs.is_open())
			{
				co_return virgo::http_status::not_found;
			}

			auto file_size = std::filesystem::file_size(file_path);

			response().set_field("Content-Length", std::to_string(file_size));

			auto ext = std::filesystem::path(file_path).extension();

			response().set_field("Content-Type", content_type);

			auto& stream = response().body().stream_;
			stream.resize(file_size);
			ifs.read(stream.data(), stream.size());

			co_return virgo::http_status::ok;
		}
	};
} // namespace aquarius