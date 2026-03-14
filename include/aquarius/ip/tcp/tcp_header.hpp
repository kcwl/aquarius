#pragma once
#include <cstdint>

namespace aquarius
{
	class tcp_header
	{
	public:
		tcp_header()
			: uuid_(0)
			, timestamp_(0)
		{}

		virtual ~tcp_header() = default;

		tcp_header(const tcp_header& other)
			: uuid_(other.uuid_)
			, timestamp_(other.timestamp_)
		{}

		tcp_header& operator=(const tcp_header& other)
		{
			if (this != std::addressof(other))
			{
				uuid_ = other.uuid_;
				timestamp_ = other.timestamp_;
			}

			return *this;
		}

		tcp_header(tcp_header&& other) noexcept
			: uuid_(std::exchange(other.uuid_, 0))
			, timestamp_(std::exchange(other.timestamp_, 0))
		{}

		tcp_header& operator=(tcp_header&& other) noexcept
		{
			if (std::addressof(other) != this)
			{
				uuid_ = std::exchange(other.uuid_, 0);
				timestamp_ = std::exchange(other.timestamp_, 0);
			}

			return *this;
		}

	public:
		virtual error_code serialize(flex_buffer& buffer)
		{
			parse_.to_datas(uuid_, buffer);

			parse_.to_datas(timestamp_, buffer);

			return error_code{};
		}

		virtual error_code deserialize(flex_buffer& buffer)
		{
			uuid_ = parse_.from_datas<uint64_t>(buffer);

			timestamp_ = parse_.from_datas<int64_t>(buffer);

			return error_code{};
		}

	public:
		uint64_t uuid() const
		{
			return uuid_;
		}
		uint64_t& uuid()
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
		int64_t& timestamp()
		{
			return timestamp_;
		}
		void timestamp(int64_t value)
		{
			timestamp_ = value;
		}

	private:
		uint64_t uuid_;

		int64_t timestamp_;

		binary_parse parse_;
	};

} // namespace aquarius