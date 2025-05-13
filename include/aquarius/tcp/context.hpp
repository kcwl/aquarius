#pragma once
#include <aquarius/detail/auto_register.hpp>
#include <aquarius/tcp/context_base.hpp>
#include <aquarius/tcp/package_processor.hpp>

namespace aquarius
{
	template <typename Message>
	class basic_context : public context_base
	{
	public:
		basic_context(const std::string& name, const std::chrono::milliseconds timeout)
			: context_base(name, timeout)
		{}

	public:
		template <typename Session>
		int visit(std::shared_ptr<Message> msg, std::shared_ptr<Session> session_ptr)
		{
			message_ptr_ = msg;

			return this->handle();
		}

		std::shared_ptr<Message> message()
		{
			return message_ptr_;
		}

	protected:
		virtual int handle() = 0;

	protected:
		std::shared_ptr<Message> message_ptr_;
	};

	template <typename Request, typename Response>
	class server_context : public basic_context<Request>
	{
		using base_type = basic_context<Request>;

	public:
		server_context(const std::string& name, const std::chrono::milliseconds& timeout = 100ms)
			: base_type(name, timeout)
		{}

	public:
		template <typename Session>
		int visit(std::shared_ptr<Request> msg, std::shared_ptr<Session> session_ptr)
		{
			auto result = base_type::visit(msg, session_ptr);

			send_response(result, session_ptr);

			return result;
		}

	public:
		Response& response()
		{
			return response_;
		}

	protected:
		template <typename Session>
		void send_response(int result, std::shared_ptr<Session> session_ptr)
		{
			if (!session_ptr)
				return;

			auto fs = make_response(result);

			boost::asio::co_spawn(session_ptr->get_executor(), session_ptr->send_packet(Response::Number, std::move(fs)),
								  boost::asio::detached);
		}

		flex_buffer_t make_response(int result)
		{
			response_.set_result(result);

			return response_.template to_binary<flex_buffer_t>();
		}

	protected:
		Response response_;
	};

	template <typename Response>
	class client_context : public basic_context<Response>
	{
		using base_type = basic_context<Response>;

	public:
		client_context(const std::string& name, const std::chrono::milliseconds& timeout = 100ms)
			: base_type(name, timeout)
		{}
	};
} // namespace aquarius

#define AQUARIUS_HANDLE_SUITE(suite_name)                                                                              \
	namespace suite_name                                                                                               \
	{

#define AQUARIUS_HANDLE_SUITE_END() }

#define AQUARIUS_GLOBAL_ID(request) #request

namespace aquarius
{
	template <typename IO /*= tcp::async_io_service<ssl::no_ssl_protocol>*/, template<typename> typename Processor>
	class basic_session;
}

#define AQUARIUS_SERVER_CONTEXT(method, __request, __response)                                                         \
	class method;                                                                                                      \
	[[maybe_unused]] static aquarius::auto_register<aquarius::basic_session<aquarius::tcp::async_io_service<aquarius::ssl::no_ssl_protocol>, aquarius::package_processor>, __request, method>                      \
		__auto_register_##method(__request::Number);                                                                   \
	class method : public aquarius::server_context<__request, __response>                                              \
	{                                                                                                                  \
	public:                                                                                                            \
		using request_type = __request;                                                                                \
		using base_type = aquarius::server_context<__request, __response>;                                             \
                                                                                                                       \
	public:                                                                                                            \
		method()                                                                                                       \
			: base_type(AQUARIUS_GLOBAL_ID(__context_##__request))                                                     \
		{}                                                                                                             \
                                                                                                                       \
	protected:                                                                                                         \
		virtual int handle() override;                                                                                 \
	};                                                                                                                 \
	inline int method::handle()																						   

#define AQUARIUS_CLIENT_CONTEXT(method, __response)                                                                    \
	class method;                                                                                                      \
	[[maybe_unused]] static aquarius::auto_register<aquarius::basic_session<aquarius::tcp::async_io_service<aquarius::ssl::no_ssl_protocol>, aquarius::package_processor>, __response, method>                     \
		__auto_register_##method(__response::Number);                                                                  \
	class method : public aquarius::client_context<__response>                                                         \
	{                                                                                                                  \
	public:                                                                                                            \
		using base_type = aquarius::client_context<__response>;                                                        \
	public:                                                                                                            \
		method()                                                                                                       \
			: base_type(AQUARIUS_GLOBAL_ID(__context_##__response))                                                    \
		{}                                                                                                             \
	protected:                                                                                                         \
		virtual int handle() override;                                                                                 \
	};                                                                                                                 \
	inline int method::handle()																						   