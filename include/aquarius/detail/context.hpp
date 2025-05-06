#pragma once
#include <aquarius/detail/auto_register.hpp>
#include <aquarius/detail/context_base.hpp>

namespace aquarius
{
	template <typename Request, typename Response>
	class context : public context_base, public visitor<Request>
	{
	public:
		context(const std::string& name, const std::chrono::milliseconds& timeout = 100ms)
			: context_base(name, timeout)
			, request_ptr_(nullptr)
			, response_()
		{}

	public:
		virtual int visit(Request* req, std::shared_ptr<session_base> session_ptr)
		{
			request_ptr_ = req;

			session_ptr_ = session_ptr;

			int result = this->handle();

			send_response(result);

			return result;
		}

	protected:
		virtual int handle() = 0;

		bool send_response(int result)
		{
			auto fs = make_response(result);

			this->session_ptr_->send_packet(std::move(fs));

			return true;
		}

	private:
		flex_buffer_t make_response(int result)
		{
			response_.set_uuid(request_ptr_->uuid());

			response_.header()->set_result(result);

			flex_buffer_t fs;

			error_code ec{};

			response_.to_binary(fs, ec);

			return fs;
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

#define AQUARIUS_HANDLE_METHOD(method, request, response)                                                              \
	class method;                                                                                                      \
	[[maybe_unused]] static aquarius::auto_register<method> __auto_register_##method(AQUARIUS_GLOBAL_ID(request));     \
	class method : public aquarius::context<request, response>                                                         \
	{                                                                                                                  \
	public:                                                                                                            \
		using request_type = request;                                                                                  \
                                                                                                                       \
	public:                                                                                                            \
		method() = default;                                                                                            \
                                                                                                                       \
	protected:                                                                                                         \
		virtual void handle() override;                                                                                \
	};                                                                                                                 \
	void method::handle()                                                                                              \