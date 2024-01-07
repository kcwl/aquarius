#pragma once
#include <aquarius/defines.hpp>
#include <cstdint>

namespace aquarius
{
	constexpr static int32_t unknown_req_number = 0;
	constexpr static int32_t unknown_resp_number = 1;
	constexpr static int32_t message_base_number = 1000;

	struct command_header
	{
		void clone(const command_header& header)
		{
			total_seq_ = header.total_seq_;
			sequences_ = header.sequences_;
			reserve_ = header.reserve_;
			size_ = header.size_;
		}

		uint32_t proxy_proto_;
		int32_t total_seq_;
		int32_t sequences_;
		uint32_t reserve_;
		uint32_t size_;
	};

	struct request_header : command_header
	{
		uint32_t uid_;
		uint32_t session_id_;
	};

	struct response_header : command_header
	{
		int32_t result_;
	};
} // namespace aquarius