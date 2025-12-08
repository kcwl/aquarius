#pragma once
#include <aquarius/serialize/http_json_serialize.hpp>

namespace aquarius
{
	class http_request_header : public http_json_serialize
	{
	public:
		http_request_header() = default;
		virtual ~http_request_header() = default;

		bool operator==(const http_request_header&) const
		{
			return true;
		}

	public:
		virtual void serialize(flex_buffer&) override
		{
			return;
		}

		virtual void deserialize(flex_buffer&) override
		{
			return;
		}
	};

	class http_response_header : public http_json_serialize
	{
	public:
		http_response_header() = default;
		virtual ~http_response_header() = default;

		bool operator==(const http_response_header&) const
		{
			return true;
		}

	public:
		virtual void serialize(flex_buffer&) override
		{
			return;
		}

		virtual void deserialize(flex_buffer&) override
		{
			return;
		}
	};

	inline std::ostream& operator<<(std::ostream& os, const http_request_header&)
	{
		return os;
	}

	inline std::ostream& operator<<(std::ostream& os, const http_response_header&)
	{
		return os;
	}
} // namespace aquarius