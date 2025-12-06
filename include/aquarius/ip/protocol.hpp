#pragma once
#include <aquarius/asio.hpp>

namespace aquarius
{
	enum class proto_tag
	{
		tcp = 0,
		http = 1,
	};

	inline std::string from_tag_string(proto_tag tag)
	{
		std::string result = "none";

		switch (tag)
		{
		case proto_tag::http:
			result = "http";
			break;
		case proto_tag::tcp:
			result = "tcp";
			break;
		default:
			break;
		}

		return result;
	}

	template <typename T>
	struct handler_tag_traits;

	template <auto Tag>
	struct protocol
	{
		using socket = boost::asio::ip::tcp::socket;

		using endpoint = boost::asio::ip::tcp::endpoint;

		using acceptor = boost::asio::ip::tcp::acceptor;

		using resolver = boost::asio::ip::tcp::resolver;
	};
} // namespace aquarius