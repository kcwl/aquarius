#pragma once
#include <aquarius/serialize/http_cookie_serialize.hpp>

namespace aquarius
{
	class http_request_header : public http_cookie_serialize
	{
		using base = http_cookie_serialize;

	public:
		http_request_header()
			: base()
			, uuid_()
		{}

		virtual ~http_request_header() = default;

		http_request_header(const http_request_header& other)
			: base(other)
			, uuid_(other.uuid_)
		{}

		http_request_header& operator=(const http_request_header& other)
		{
			if (this != std::addressof(other))
			{
				base::operator=(other);
				uuid_ = other.uuid_;
			}

			return *this;
		}

		http_request_header(http_request_header&& other) noexcept
			: base(std::move(other))
			, uuid_(std::exchange(other.uuid_, 0))
		{}

		http_request_header& operator=(http_request_header&& other) noexcept
		{
			if (this != std::addressof(other))
			{
				base::operator=(std::move(other));
				uuid_ = std::exchange(other.uuid_, 0);
			}

			return *this;
		}

		bool operator==(const http_request_header& other) const
		{
			return uuid_ == other.uuid_;
		}

	public:
		virtual void serialize(flex_buffer& buffer) override
		{
			this->parse_to(uuid_, buffer, "uuid");
		}

		virtual void deserialize(flex_buffer& buffer) override
		{
			uuid_ = this->parse_from<uint64_t>(buffer, "uuid");
		}

		uint64_t uuid() const
		{
			return uuid_;
		}

		void uuid(uint64_t v)
		{
			uuid_ = v;
		}

	private:
		uint64_t uuid_;
	};

	class http_response_header : public http_request_header
	{
		using base = http_request_header;

	public:
		http_response_header()
			: result_()
		{}

		virtual ~http_response_header() = default;

		http_response_header(const http_response_header& other)
			: base(other)
			, result_(other.result_)
		{}

		http_response_header& operator=(const http_response_header& other)
		{
			if (this != std::addressof(other))
			{
				base::operator=(other);
				result_ = other.result_;
			}

			return *this;
		}

		http_response_header(http_response_header&& other) noexcept
			: base(std::move(other))
			, result_(std::exchange(other.result_, 0))
		{}

		http_response_header& operator=(http_response_header&& other) noexcept
		{
			if (this != std::addressof(other))
			{
				base::operator=(std::move(other));
				result_ = std::exchange(other.result_, 0);
			}

			return *this;
		}

		bool operator==(const http_response_header& other) const
		{
			return result_ == other.result_;
		}

	public:
		virtual void serialize(flex_buffer& buffer) override
		{
			base::serialize(buffer);
			buffer.sputc(',');
			this->parse_to(result_, buffer, "result");
		}

		virtual void deserialize(flex_buffer& buffer) override
		{
			base::deserialize(buffer);
			char c{};
			buffer.sgetn(&c, sizeof(char));
			if (c != ',')
			{
				return;
			}
			result_ = base::parse_from<int32_t>(buffer, "result");
		}

		int32_t result() const
		{
			return result_;
		}

		void result(int32_t v)
		{
			result_ = v;
		}

	private:
		int32_t result_;
	};

	inline std::ostream& operator<<(std::ostream& os, const http_request_header& header)
	{
		os << "uuid: " << header.uuid();

		return os;
	}

	inline std::ostream& operator<<(std::ostream& os, const http_response_header& header)
	{
		os << "result: " << header.result();

		return os;
	}
} // namespace aquarius