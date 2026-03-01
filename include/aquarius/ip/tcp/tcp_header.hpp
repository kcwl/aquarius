#pragma once
#include <aquarius/serialize/tcp_binary_serialize.hpp>
#include <cstdint>

namespace aquarius
{
	class tcp_request_header : public tcp_binary_serialize
	{
		using base = tcp_binary_serialize;

	public:
		tcp_request_header()
			: base()
			, uuid_(0)
			, timestamp_(0)
			, version_(0)
		{}

		virtual ~tcp_request_header() = default;

		tcp_request_header(const tcp_request_header& other)
			: base(other)
			, uuid_(other.uuid_)
			, timestamp_(other.timestamp_)
			, version_(other.version_)
		{}

		tcp_request_header& operator=(const tcp_request_header& other)
		{
			if (this != std::addressof(other))
			{
				base::operator=(other);
				timestamp_ = other.timestamp_;
				version_ = other.version_;
			}

			return *this;
		}

		tcp_request_header(tcp_request_header&& other) noexcept
			: base(std::move(other))
			, timestamp_(std::exchange(timestamp_, 0))
			, version_(std::exchange(version_, 0))
		{}

		tcp_request_header& operator=(tcp_request_header&& other) noexcept
		{
			if (std::addressof(other) != this)
			{
				base::operator=(std::move(other));

				timestamp_ = std::exchange(timestamp_, 0);
				version_ = std::exchange(version_, 0);
			}

			return *this;
		}

		bool operator==(const tcp_request_header& other) const
		{
			return uuid_ == other.uuid_;
		}

		std::ostream& operator<<(std::ostream& os) const
		{
			os << "uuid: " << uuid_ << ", timestamp: " << timestamp_ << ", version: " << version_;
			return os;
		}

	public:
		virtual void serialize(flex_buffer& buffer) override
		{
			this->parse_to(uuid_, buffer);

			this->parse_to(timestamp_, buffer);

			this->parse_to(version_, buffer);
		}

		virtual void deserialize(flex_buffer& buffer) override
		{
			uuid_ = this->parse_from<uint64_t>(buffer);

			timestamp_ = this->parse_from<int64_t>(buffer);

			version_ = this->parse_from<int32_t>(buffer);
		}

	public:
		uint64_t uuid() const
		{
			return uuid_;
		}

		void uuid(uint64_t value)
		{
			uuid_ = value;
		}

		int64_t timestamp() const
		{
			return timestamp_;
		}

		int32_t version() const
		{
			return version_;
		}

		void timestamp(int64_t value)
		{
			timestamp_ = value;
		}

		void version(int32_t value)
		{
			version_ = value;
		}

	private:
		uint64_t uuid_;

		int64_t timestamp_;

		int32_t version_;
	};

	class tcp_response_header : public tcp_request_header
	{
		using base = tcp_request_header;

	public:
		tcp_response_header()
			: base()
			, result_()
		{}

		virtual ~tcp_response_header() = default;

		tcp_response_header(const tcp_response_header& other)
			: base(other)
			, result_(other.result_)
		{}

		tcp_response_header& operator=(const tcp_response_header& other)
		{
			if (this != std::addressof(other))
			{
				base::operator=(other);
				result_ = other.result_;
			}

			return *this;
		}

		tcp_response_header(tcp_response_header&& other) noexcept
		{
			if (this != std::addressof(other))
			{
				base::operator=(std::move(other));
				result_ = std::exchange(other.result_, 0);
			}
		}

		bool operator==(const tcp_response_header& other) const
		{
			return base::operator==(other) && result_ == other.result_;
		}

		std::ostream& operator<<(std::ostream& os) const
		{
			base::operator<<(os);

			os << ", result: " << result_;

			return os;
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
		int result() const
		{
			return result_;
		}

		void result(int v)
		{
			result_ = v;
		}

	public:
		int result_;
	};

	inline std::ostream& operator<<(std::ostream& os, const tcp_request_header& header)
	{
		header << os;

		return os;
	}

	inline std::ostream& operator<<(std::ostream& os, const tcp_response_header& header)
	{
		header << os;

		return os;
	}

} // namespace aquarius