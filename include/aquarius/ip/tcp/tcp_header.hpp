#pragma once
#include <cstdint>
#include <aquarius/serialize/tcp_binary_serialize.hpp>

namespace aquarius
{
	class tcp_request_header : public tcp_binary_serialize
	{
	public:
		tcp_request_header() = default;
		virtual ~tcp_request_header() = default;

		bool operator==(const tcp_request_header& other) const
		{
			return uuid_ == other.uuid_;
		}

	public:
		virtual void serialize(flex_buffer& buffer) override
		{
			this->parse_to(uuid_, buffer);
		}

		virtual void deserialize(flex_buffer& buffer) override
		{
			uuid_ = this->parse_from<uint64_t>(buffer);
		}

	public:
		uint64_t uuid_;
	};

	class tcp_response_header : public tcp_request_header
	{
	public:
		tcp_response_header() = default;
		virtual ~tcp_response_header() = default;

		bool operator==(const tcp_response_header& other) const
		{
			return tcp_request_header::operator==(other) && result_ == other.result_;
		}

	public:
		virtual void serialize(flex_buffer& buffer) override
		{
			tcp_request_header::serialize(buffer);

			this->parse_to(result_, buffer);
		}

		virtual void deserialize(flex_buffer& buffer) override
		{
			tcp_request_header::deserialize(buffer);

			result_ = this->parse_from<int>(buffer);
		}

	public:
		int result_;
	};

	inline std::ostream& operator<<(std::ostream& os, const tcp_request_header& header)
	{
		os << header.uuid_;

		return os;
	}

	inline std::ostream& operator<<(std::ostream& os, const tcp_response_header& header)
	{
		os << header.uuid_ << header.result_;

		return os;
	}

} // namespace aquarius