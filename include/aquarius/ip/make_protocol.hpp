#pragma once
#include <aquarius/serialize/flex_buffer.hpp>
#include <aquarius/virgo/http_method.hpp>
#include <aquarius/virgo/http_version.hpp>
#include <aquarius/serialize/binary.hpp>

namespace aquarius
{

	template <bool Request, typename Message>
	void make_http_buffer(Message& msg, flex_buffer& buffer)
	{
		flex_buffer temp{};


		

		std::string headline{};

		if constexpr (Request)
		{
			if constexpr (Message::method == virgo::http_method::get)
			{
				flex_buffer tempget;
				msg.body().serialize(tempget);
				std::string temp_str;
				tempget.get(temp_str);
				headline = std::format("{} /{}{} {}\r\n", virgo::from_method_string(Message::method), Message::router,
									   temp_str, virgo::from_version_string(msg.version()));
			}
			else
			{
				msg.commit(temp);

				headline = std::format("{} {} {}\r\n", virgo::from_method_string(Message::method), Message::router,
									   virgo::from_version_string(msg.version()));
			}
		}
		else
		{
			msg.commit(temp);

			headline = std::format("{} {} {}\r\n", from_version_string(msg.version()), static_cast<int>(msg.result()),
								   msg.reason().data());
		}

		for (auto& s : msg.fields())
		{
			headline += std::format("{}: {}\r\n", s.first, s.second);
		}

		headline += "\r\n";

		buffer.put(headline.begin(), headline.end());
		
		buffer.append(std::move(temp));
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

	template <bool Request, typename Message>
	void make_tcp_buffer(Message& msg, flex_buffer& buffer)
	{
		constexpr auto pos = sizeof(uint32_t);

		buffer.commit(pos);

		if constexpr (Request)
		{
			binary_parse{}.to_datas(Message::router, buffer);
		}

		msg.commit(buffer);

		auto len = static_cast<uint32_t>(buffer.tellg() - pos);
		std::copy((char*)&len, (char*)(&len + 1), buffer.data());
	}
} // namespace aquarius