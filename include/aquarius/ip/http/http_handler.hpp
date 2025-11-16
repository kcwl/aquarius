#pragma once
#include <aquarius/basic_handler.hpp>
#include <aquarius/virgo/http_method.hpp>
#include <aquarius/serialize/flex_buffer.hpp>
#include <aquarius/virgo/http_status.hpp>

namespace aquarius
{
	template <typename Session, typename Request, typename Response>
	class basic_http_handler : public basic_handler<Session, Request, Response>
	{
	public:
		basic_http_handler(const std::string& name)
			: basic_handler<Session, Request, Response>(name)
		{}

	public:
		virtual auto visit(std::shared_ptr<Session> session_ptr, std::shared_ptr<Request> request_ptr)
			-> awaitable<error_code> override
		{
			this->session_ptr_ = session_ptr;
			this->request_ptr_ = request_ptr;

			this->response_.version(this->request()->version());

			 this->response_.set_field("Content-Type", this->request()->find("Content-Type"));
			this->response_.set_field("Server", "Aquarius 0.10.0");
			this->response_.set_field("Connection", this->request()->find("Connection"));
			this->response_.set_field("Access-Control-Allow-Origin", "*");

			auto ec = co_await this->handle();

			co_return co_await this->send_response(ec);
		}

		virtual auto send_response(error_code ec) -> awaitable<error_code>
		{
			// this->response_.result(ec.value());

			// this->response_.reason(virgo::from_status_string(ec.value()));

			// flex_buffer buffer{};

			// make_http_buffer<false>(this->response(), buffer, Method);

			// co_return co_await this->session()->async_send(std::move(buffer));

			flex_buffer buffer{};
			flex_buffer temp{};

			this->response_.commit(temp);

			this->response_.version(virgo::http_version::http1_1);

			auto headline = std::format("{} {} {}\r\n", virgo::from_string_version(this->response_.version()),
										static_cast<int>(this->response_.result()),
										virgo::from_status_string(this->response_.result()).data());

			for (auto& s : this->response_.fields())
			{
				headline += std::format("{}: {}\r\n", s.first, s.second);
			}

			headline += "\r\n";

			buffer.sputn(headline.c_str(), headline.size());

			if (temp.size() != 0)
			{
				buffer.sputn((char*)temp.data().data(), temp.data().size());
			}

			co_return co_await this->session_ptr_->async_send(buffer);
		}
	};

	template <typename Session, typename Handler, typename Router, virgo::http_method Method>
	struct auto_http_handler_register
	{
		explicit auto_http_handler_register(std::string_view proto)
		{
			Router::get_mutable_instance().template regist<Method, Handler>(proto);
		}
	};
} // namespace aquarius

#define __AQUARIUS_HTTP_HANDLER(__session, __method, __request, __response)                                            \
	class __method final : public aquarius::basic_http_handler<__session, __request, __response>                       \
	{                                                                                                                  \
	public:                                                                                                            \
		using base_type = aquarius::basic_http_handler<__session, __request, __response>;                              \
                                                                                                                       \
	public:                                                                                                            \
		__method()                                                                                                     \
			: base_type(AQUARIUS_GLOBAL_STR_ID(__handler_##__method))                                                  \
		{}                                                                                                             \
		virtual auto handle() -> aquarius::awaitable<aquarius::error_code> override;                                   \
	};                                                                                                                 \
	inline auto __method::handle() -> aquarius::awaitable<aquarius::error_code>

#define AQUARIUS_CONTEXT_BY_HTTP(__session, __request, __response, __method, __router, __http_method)                  \
	class __method;                                                                                                    \
	[[maybe_unused]] static aquarius::auto_http_handler_register<__session, __method, __router, __http_method>         \
		__auto_register_##__method(__request::router);                                                                 \
	__AQUARIUS_HTTP_HANDLER(__session, __method, __request, __response)