#pragma once
#include <aquarius/http.hpp>
#include <aquarius/tcp.hpp>
#include <aquarius/virgo/http_request.hpp>
#include <aquarius/virgo/tcp_request.hpp>
#include <aquarius/virgo/tcp_response.hpp>

namespace aquarius
{
	template <typename Message, typename Handler>
	struct context_traits;

	template <auto Router, typename Body, typename Handler>
	struct context_traits<tcp_request<Router, Body>, Handler>
	{
		using type = tcp::context<Handler>;
	};

	template <auto Router, typename Body, typename Handler>
	struct context_traits<tcp_response<Router, Body>, Handler>
	{
		using type = tcp::context<Handler>;
	};

	template <auto Router, typename Body, typename Handler>
	struct context_traits<http_request<Router, Body>, Handler>
	{
		using type = http::context<Handler>;
	};

	// template <auto Router, typename Body, typename Handler>
	// struct context_traits<http_response<Router, Body>, Handler>
	//{
	//	using type = http::context<Handler>;
	// };
} // namespace aquarius