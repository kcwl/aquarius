#pragma once
#include <aquarius/defines.hpp>
#include <cstdint>
#include <aquarius/elastic.hpp>

namespace aquarius
{
	constexpr static int32_t unknown_req_number = 0;
	constexpr static int32_t unknown_resp_number = 1;
	constexpr static int32_t message_base_number = 1000;

	struct tcp_header
	{
		void clone(const tcp_header& header)
		{
			uid = header.uid;
			proxy = header.proxy;
			src = header.src;
			reserve = header.reserve;
		}

		uint64_t proxy;
		uint64_t uid;
		uint64_t src;
		uint64_t size;
		uint64_t reserve;
	};

	struct tcp_request_header : tcp_header
	{
		uint32_t session_id;

	private:
		friend class elastic::access;

		template<typename _Archive>
		void serialize(_Archive& ar)
		{
			ar& elastic::base_object<tcp_header>(*this);

			ar& session_id;
		}
	};

	struct tcp_response_header : tcp_header
	{
		int32_t result;

	private:
		friend class elastic::access;

		template<typename _Archive>
		void serialize(_Archive& ar)
		{
			ar& elastic::base_object<tcp_header>(*this);

			ar& result;
		}
	};
} // namespace aquarius