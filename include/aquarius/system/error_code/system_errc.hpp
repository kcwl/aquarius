#pragma once
#include <string>

namespace aquarius
{
	enum class system_errc
	{
		ok,
		invalid_proto,
		wait_for_query,
		invalid_stream,
		invalid_context,
		invalid_message
	};

	namespace system
	{
		std::string system_errc_map(int error)
		{
			auto errc = static_cast<system_errc>(error);

			switch (errc)
			{
			case aquarius::system_errc::ok:
				return "success";
			case aquarius::system_errc::invalid_proto:
				return "maybe the protocal is not registed";
			case aquarius::system_errc::wait_for_query:
				return "the buffer is not completed";
			case aquarius::system_errc::invalid_stream:
				return "the stream is not completed";
			case aquarius::system_errc::invalid_context:
				return "maybe context is not complete";
			case aquarius::system_errc::invalid_message:
				return "maybe the message is invalid";
			default:
				break;
			}

			return {};
		}
	}
}