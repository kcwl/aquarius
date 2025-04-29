#pragma once
#include <aquarius/detail/handle_method_base.hpp>
#include <aquarius/detail/auto_register.hpp>

namespace aquarius
{
	template <typename Request, typename Response>
	class handle_method : public handle_method_base, public method_visitor<Request>
	{
	public:
		handle_method() = default;

	public:
		virtual void visit(std::unique_ptr<Request> req, flex_buffer_t& ar) override
		{
			request_ptr_ = req;

			handle();

			resp_.to_binary(ar);
		}

	protected:
		virtual void handle() = 0;

	private:
		std::unique_ptr<Request> request_ptr_;

		Response resp_;
	};
} // namespace aquarius

#define AQUARIUS_HANDLE_SUITE(suite_name) \
namespace suite_name \
{ \

#define AQUARIUS_HANDLE_SUITE_END() }

#define AQUARIUS_GLOBAL_ID(request) #request

#define AQUARIUS_HANDLE_METHOD(method, request, response)                                                              \
	class method;                                                                                                      \
	[[maybe_unused]] static aquarius::auto_register<method> __auto_register_##method(AQUARIUS_GLOBAL_ID(request));     \
	class method : public aquarius::handle_method<request, response>                                                   \
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