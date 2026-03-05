#pragma once
#include <aquarius/ip/handler.hpp>
#include <aquarius/ip/http/http_method_define.hpp>
#include <aquarius/ip/http/http_server.hpp>
#include <aquarius/virgo/http_method.hpp>
#include <aquarius/virgo/http_null_protocol.hpp>
#include <vector>

namespace aquarius
{
	using options_request = virgo::http_request<"OPTIONS", virgo::http_method::options, virgo::http_null_body>;
	using options_response = virgo::http_response<virgo::http_method::options, virgo::http_null_body>;

	class http_options_method_handler : public handler<options_request, options_response>
	{
		using base = handler<options_request, options_response>;

	public:
		http_options_method_handler()
			: base(std::string(__http_options_handler__))
		{}

	public:
		virtual auto handle() -> awaitable<error_code> override
		{
			this->response().header().set_field("Access-Control-Allow-Origin", co_await mpc_http_origin());
			this->response().header().set_field("Access-Control-Request-Method", "POST");
			this->response().header().set_field("Access-Control-Allow-Headers", "Aquarius-Header");
			this->response().header().set_field("Access-Control-Max-Age", "31536000");
			this->response().header().set_field("Access-Control-Allow-Credentials", "true");

			co_return virgo::http_status::ok;
		}
	};

	class http_stream_body : public http_json_serialize
	{
	public:
		http_stream_body()
			: stream()
		{}
		virtual ~http_stream_body() = default;

	public:
		bool operator==(const http_stream_body& other)
		{
			return stream == other.stream;
		}

	public:
		virtual void serialize(aquarius::flex_buffer& buffer) override
		{
			this->parse_to(stream, buffer);
		}

		virtual void deserialize(aquarius::flex_buffer& buffer) override
		{
			stream = this->parse_from<std::vector<char>>(buffer);
		}

	public:
		std::vector<char> stream;
	};

	using resource_request = virgo::http_request<"GET", virgo::http_method::get, virgo::http_null_body>;
	using resource_response = virgo::http_response<virgo::http_method::get, http_stream_body>;

	class http_source_method_handler : public handler<resource_request, resource_response>
	{
		using base = handler<resource_request, resource_response>;

	public:
		http_source_method_handler()
			: base(std::string(__http_source_handler__))
		{}

	public:
		virtual auto handle() -> awaitable<error_code> override
		{
			auto router = this->request()->header().find("router");

			auto file_path = (co_await mpc_http_root()) + std::string(router.data(), router.size());

			std::fstream ifs(file_path, std::ios::in | std::ios::binary);
			if (!ifs.is_open())
			{
				co_return virgo::http_status::not_found;
			}

			auto file_size = std::filesystem::file_size(file_path);

			this->response().header().set_field("Content-Length", std::to_string(file_size));

			auto ext = std::filesystem::path(file_path).extension();

			this->response().header().set_field("Content-Type", this->request()->header().content_type());

			auto& stream = this->response().body().stream;
			stream.resize(file_size);
			ifs.read(stream.data(), stream.size());

			co_return virgo::http_status::ok;
		}
	};

	[[maybe_unused]] inline static aquarius::auto_handler_register<
		http_options_method_handler> __auto_register_options_handler(__http_options_handler__);
	[[maybe_unused]] inline static aquarius::auto_handler_register<
		http_source_method_handler> __auto_register_source_handler(__http_source_handler__);
} // namespace aquarius