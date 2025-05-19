#pragma once
#include <aquarius/server_context.hpp>
#include <aquarius/client_context.hpp>
#include <aquarius/tcp/session.hpp>

#define AQUARIUS_HANDLE_SUITE(suite_name)                                                                              \
	namespace suite_name                                                                                               \
	{

#define AQUARIUS_HANDLE_SUITE_END() }

#define AQUARIUS_GLOBAL_ID(request) #request

#define AQUARIUS_SERVER_CONTEXT(method, __request, __response)                                                         \
	class method;                                                                                                      \
	[[maybe_unused]] static aquarius::tcp::auto_register<aquarius::tcp::session, __request, method>                    \
		__auto_register_##method(__request::Number);                                                                   \
	class method : public aquarius::server_context<__request, __response>											   \
	{                                                                                                                  \
	public:                                                                                                            \
		using request_type = __request;                                                                                \
		using base_type = aquarius::server_context<__request, __response>;		                                       \
	public:                                                                                                            \
		method()                                                                                                       \
			: base_type(AQUARIUS_GLOBAL_ID(__context_##__request))                                                     \
		{}                                                                                                             \
	protected:                                                                                                         \
		virtual int handle() override;                                                                                 \
	};                                                                                                                 \
	inline int method::handle()

#define AQUARIUS_CLIENT_CONTEXT(method, __response)                                                                    \
	class method;                                                                                                      \
	[[maybe_unused]] static aquarius::tcp::auto_register<aquarius::tcp::session, __response, method>                   \
		__auto_register_##method(__response::Number);                                                                  \
	class method : public aquarius::client_context<__response>		                                                   \
	{                                                                                                                  \
	public:                                                                                                            \
		using base_type = aquarius::client_context<__response>;														   \
	public:                                                                                                            \
		method()                                                                                                       \
			: base_type(AQUARIUS_GLOBAL_ID(__context_##__response))                                                    \
		{}                                                                                                             \
	protected:                                                                                                         \
		virtual int handle() override;                                                                                 \
	};                                                                                                                 \
	inline int method::handle()