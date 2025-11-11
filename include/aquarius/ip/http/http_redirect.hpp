#pragma once
#include <aquarius/virgo/http_request.hpp>
#include <aquarius/virgo/http_response.hpp>
#include <aquarius/virgo/http_null_body.hpp>
#include <aquarius/virgo/http_method.hpp>
#include <aquarius/ip/http/http_header.hpp>
#include <aquarius/ip/http/http_handler.hpp>

#define STRING(value) #value

#define AQUAIRUS_REDIRECT_CAT(src) STRING(__system_redirect_##src)

#define REDIRECT_MESSAGE(name, msg) name##_redirect_##msg

#define AQUARIUS_CAT(a, b) a##_##b

#define ROUTER_0() root
#define ROUTER_1(r) AQUARIUS_CAT(ROUTER_0(), r)
#define ROUTER_2(r, ...) AQUARIUS_CAT(ROUTER_1(__VA_ARGS__), r)
#define ROUTER_3(r, ...) AQUARIUS_CAT(ROUTER_2(__VA_ARGS__), r)
#define ROUTER_4(r, ...) AQUARIUS_CAT(ROUTER_3(__VA_ARGS__), r)
#define ROUTER_5(r, ...) AQUARIUS_CAT(ROUTER_4(__VA_ARGS__), r)
#define ROUTER_6(r, ...) AQUARIUS_CAT(ROUTER_5(__VA_ARGS__), r)
#define ROUTER_7(r, ...) AQUARIUS_CAT(ROUTER_6(__VA_ARGS__), r)
#define ROUTER_8(r, ...) AQUARIUS_CAT(ROUTER_7(__VA_ARGS__), r)
#define ROUTER_9(r, ...) AQUARIUS_CAT(ROUTER_8(__VA_ARGS__), r)
#define ROUTER_10(r, ...) AQUARIUS_CAT(ROUTER_9(__VA_ARGS__), r)

#define AQUARIUS_REDIRECT_MESSAGE(src, dest)                                                                           \
	using REDIRECT_MESSAGE(src, request) =                                                                             \
		aquarius::virgo::http_request<AQUAIRUS_REDIRECT_CAT(src), aquarius::http_request_header,                       \
									  aquarius::virgo::http_null_body>;                                                \
	using REDIRECT_MESSAGE(src, response) =                                                                            \
		aquarius::virgo::http_response<AQUAIRUS_REDIRECT_CAT(src), aquarius::http_response_header,                     \
									   aquarius::virgo::http_null_body>;

#define AQUARIUS_CONTEXT_BY_REDIRECT(__session, __router, __http_method, __src, __dest)                                \
	AQUARIUS_REDIRECT_MESSAGE(__src, __dest)                                                                           \
	class __method_to_##__src;                                                                                         \
	[[maybe_unused]] static aquarius::auto_http_handler_register<                                                      \
		__session, __method_to_##__src, __router, __http_method> __auto_register_##__method(STRING(__src));            \
	class __method_to_##__src final                                                                                    \
		: public aquarius::basic_http_hander<aquarius::virgo::http_method::redirect, __session,                        \
											 REDIRECT_MESSAGE(__src, request), REDIRECT_MESSAGE(__src, response)>      \
	{                                                                                                                  \
	public:                                                                                                            \
		using base_type =                                                                                              \
			aquarius::basic_http_hander<aquarius::virgo::http_method::redirect, __session,                             \
										REDIRECT_MESSAGE(__src, request), REDIRECT_MESSAGE(__src, response)>;          \
                                                                                                                       \
	public:                                                                                                            \
		__method_to_##__src()                                                                                          \
			: base_type(AQUARIUS_GLOBAL_STR_ID(__handler_##__method))                                                  \
		{}                                                                                                             \
		virtual auto handle() -> aquarius::awaitable<aquarius::error_code> override                                    \
		{                                                                                                              \
			response().version(request()->version());                                                                  \
			response().result(aquarius::virgo::http_status::moved_permanently);                                        \
			std::string location(STRING(__dest));                                                                      \
			std::replace(location.begin(), location.end(), '_', '/');                                                  \
			auto pos = location.find_first_of('/');                                                                    \
			if (pos == std::string::npos)                                                                              \
				location = "/";                                                                                        \
			else                                                                                                       \
			{                                                                                                          \
				location = location.substr(pos);                                                                       \
			}                                                                                                          \
			response().set_field("Location", location);                                                                \
			co_return aquarius::virgo::http_status::moved_permanently;                                                 \
		}                                                                                                              \
	};

// private \
	: //template <aquarius::detail::string_literal Redirect>                                                           \
		//constexpr auto parse_router()                                                                                  \
		//{                                                                                                              \
		//	constexpr std::string_view redirect_value = aquarius::detail::bind_param<Redirect>::value;                 \
		//	constexpr auto sp = redirect_value | std::views::split('_');                                               \
		//	constexpr auto begin = sp.begin();                                                                         \
		//	constexpr auto end = sp.end();                                                                             \
		//	constexpr std::size_t sp_size = std::ranges::distance(begin, end);                                         \
		//	auto f = [&]<std::size_t... I>(std::index_sequence<I...>)                                                  \
		//	{ return aquarius::concat_v<((*(begin + I)), ...)>; };                                                     \
		//	return f(std::make_index_sequence<sp_size>());                                                             \
		//}                                                                                                              \
	};
