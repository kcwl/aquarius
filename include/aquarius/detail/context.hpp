#pragma once
#include <aquarius/detail/auto_register.hpp>
#include <aquarius/detail/context_base.hpp>

namespace aquarius
{
	template <typename Request, typename Response>
	class context : public context_base
	{
	public:
		context(const std::string& name, const std::chrono::milliseconds& timeout = 100ms)
			: context_base(name, timeout)
			, request_ptr_(nullptr)
			, response_()
		{}

	public:
		template <typename Session>
		int visit(std::shared_ptr<Request> req, std::shared_ptr<Session> session_ptr)
		{
			request_ptr_ = req;

			auto result = this->handle();

			send_response(result, session_ptr);

			return result;
		}

		std::shared_ptr<Request> request()
		{
			return request_ptr_;
		}

		Response& response()
		{
			return response_;
		}

	protected:
		virtual int handle() = 0;

		template <typename Session>
		void send_response(int result, std::shared_ptr<Session> session_ptr)
		{
			if (!session_ptr)
				return;

			auto fs = make_response(result);

			boost::asio::co_spawn(session_ptr->get_executor(), session_ptr->send_packet(Request::Number, std::move(fs)),
								  boost::asio::detached);
		}

	private:
		flex_buffer_t make_response(int result)
		{
			return response_.to_binary<flex_buffer_t>();
		}

	protected:
		std::shared_ptr<Request> request_ptr_;

		Response response_;
	};
} // namespace aquarius

#define AQUARIUS_HANDLE_SUITE(suite_name)                                                                              \
	namespace suite_name                                                                                               \
	{

#define AQUARIUS_HANDLE_SUITE_END() }

#define AQUARIUS_GLOBAL_ID(request) #request

namespace aquarius
{
	template <typename IO = async_io_service<boost::asio::ip::tcp, boost::asio::any_io_executor>>
	class basic_session;
}

#define AQUARIUS_HANDLE_METHOD(method, __request, __response)                                                          \
	class method;                                                                                                      \
	[[maybe_unused]] static aquarius::auto_register<aquarius::basic_session<>, __request, method>                      \
		__auto_register_##method(__request::Number);                                                                   \
	class method : public aquarius::context<__request, __response>                                                     \
	{                                                                                                                  \
	public:                                                                                                            \
		using request_type = __request;                                                                                \
		using base_type = aquarius::context<__request, __response>;                                                    \
	public:                                                                                                            \
		method()                                                                                                       \
			: base_type(AQUARIUS_GLOBAL_ID(__context_##__request))                                                     \
		{}                                                                                                             \
	protected:                                                                                                         \
		virtual int handle() override;                                                                                 \
	};                                                                                                                 \
	inline int method::handle()                                                                                        \