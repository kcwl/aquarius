#pragma once
#include <aquarius/serialize/flex_buffer.hpp>
#include <aquarius/virgo/http_method.hpp>
#include <aquarius/virgo/http_version.hpp>

namespace aquarius
{

	template <typename Request>
	void make_http_request_buffer(std::shared_ptr<Request> request, flex_buffer& buffer)
	{
		std::string headline{};

		if constexpr (Request::method == virgo::http_method::get)
		{
			flex_buffer temp;
			request->body().serialize(temp);
			std::string temp_str;
			temp.get(temp_str);
			headline = std::format("{} {} {}\r\n", virgo::from_method_string(Request::method), Request::router + temp_str,
								   virgo::from_version_string(request->version()));
		}
		else
		{
			headline = std::format("{} {} {}\r\n", virgo::from_method_string(Request::method), Request::router,
								   virgo::from_version_string(request->version()));
		}

		request->set_field("Content-Type", "aquarius");
		request->set_field("Server", "Aquarius 0.10.0");
		request->set_field("Connection", "keep-alive");
		request->set_field("Access-Control-Allow-Origin", "*");
		request->set_field("Content-Type", "aquarius-json");
		for (auto& s : request->fields())
		{
			headline += std::format("{}: {}\r\n", s.first, s.second);
		}
		headline += "\r\n";
		buffer.put(headline.begin(), headline.end());

		if constexpr (Request::method != virgo::http_method::get)
		{
			request->commit(buffer);
		}
	}

	template <typename... Args>
	std::string make_http_headline(Args&&... args)
	{
		std::stringstream ss{};

		((ss << args << ' '), ...);

		ss.seekp(-1, std::ios::cur);
		ss << "\r\n";

		return ss.str();
	}

	template <typename Response>
	void make_tcp_buffer(std::shared_ptr<Response> response, flex_buffer& buffer)
	{
		constexpr auto pos = sizeof(uint32_t);

		buffer.commit(pos);

		response->commit(buffer);

		auto len = static_cast<uint32_t>(buffer.tellg() - pos);
		std::copy((char*)&len, (char*)(&len + 1), buffer.data());
	}
} // namespace aquarius