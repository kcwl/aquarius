#pragma once
#include <aquarius/basic_stream_handler.hpp>
#include <aquarius/tcp/auto_register.hpp>
#include <aquarius/tcp/session.hpp>

#define AQUARIUS_GLOBAL_ID(request) #request

#define AQUARIUS_DEFAULT_HANDLER(method, __request)                                                         \
	class method;                                                                                                      \
	[[maybe_unused]] static aquarius::tcp::auto_register<aquarius::tcp::session, __request, method>                    \
		__auto_register_##method(__request::proto);                                                                    \
	class method : public aquarius::basic_handler<__request, int>                                   \
	{                                                                                                                  \
	public:                                                                                                            \
		using request_type = __request;                                                                                \
		using base_type = aquarius::basic_handler<__request, int>;                                  \
	public:                                                                                                            \
		method()                                                                                                       \
			: base_type(AQUARIUS_GLOBAL_ID(__handler_##__request))                                                     \
		{}                                                                                                             \
	protected:                                                                                                         \
		virtual int handle() override;                                                                                 \
	};                                                                                                                 \
	inline int method::handle()

#define AQUARIUS_STREAM_HANDLER(method, __request, __response)                                                         \
	class method;                                                                                                      \
	[[maybe_unused]] static aquarius::tcp::auto_register<aquarius::tcp::session, __request, method>                    \
		__auto_register_##method(__request::proto);                                                                    \
	class method : public aquarius::basic_stream_handler<__request, __response, int>                                   \
	{                                                                                                                  \
	public:                                                                                                            \
		using request_type = __request;                                                                                \
		using base_type = aquarius::basic_stream_handler<__request, __response, int>;                                  \
	public:                                                                                                            \
		method()                                                                                                       \
			: base_type(AQUARIUS_GLOBAL_ID(__handler_##__request))                                                     \
		{}                                                                                                             \
	protected:                                                                                                         \
		virtual int handle() override;                                                                                 \
	};                                                                                                                 \
	inline int method::handle()

#define AQUARIUS_BROADCAST_HANDLER(method, __response)                                                                 \
	class method;                                                                                                      \
	[[maybe_unused]] static aquarius::tcp::auto_register<aquarius::tcp::session, __response, method>                   \
		__auto_register_##method(__response::proto);                                                                   \
	class method : public aquarius::basic_handler<__response, int>                                                     \
	{                                                                                                                  \
	public:                                                                                                            \
		using base_type = aquarius::basic_handler<__response, int>;                                                    \
                                                                                                                       \
	public:                                                                                                            \
		method()                                                                                                       \
			: base_type(AQUARIUS_GLOBAL_ID(__handler_##__response))                                                    \
		{}                                                                                                             \
	protected:                                                                                                         \
		virtual int handle() override;                                                                                 \
	};                                                                                                                 \
	inline int method::handle()