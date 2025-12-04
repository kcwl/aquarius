#pragma once
#include <aquarius/basic_protocol.hpp>

namespace aquarius
{
	namespace virgo
	{
		template <bool Request, typename Header, typename Body, typename Allocator = std::allocator<Body>>
		class basic_tcp_protocol : public basic_protocol<Header, std::add_pointer_t<Body>, Allocator>
		{
		public:
			using base = basic_protocol<Header, std::add_pointer_t<Body>, Allocator>;

			using typename base::header_t;

			using typename base::body_t;

			constexpr static auto has_request = Request;

		public:
			basic_tcp_protocol()
				: base()
				, timestamp_()
				, version_()
			{}

			virtual ~basic_tcp_protocol() = default;

			basic_tcp_protocol(const basic_tcp_protocol&) = default;
			basic_tcp_protocol& operator=(const basic_tcp_protocol&) = default;

			basic_tcp_protocol(basic_tcp_protocol&& other) noexcept
				: base(std::move(other))
				, timestamp_(std::exchange(timestamp_, 0))
				, version_(std::exchange(version_, 0))
			{}

			basic_tcp_protocol& operator=(basic_tcp_protocol&& other) noexcept
			{
				if (std::addressof(other) != this)
				{
					base::operator=(std::move(other));

					timestamp_ = std::exchange(timestamp_, 0);
					version_ = std::exchange(version_, 0);
				}

				return *this;
			}

		public:
			bool operator==(const basic_tcp_protocol& other) const
			{
				return base::operator==(other) && timestamp_ == other.timestamp_ && version_ == other.version_;
			}

			std::ostream& operator<<(std::ostream& os) const
			{
				os << this->header() << " " << this->body() << " ";

				os << timestamp() << " " << version();

				return os;
			}

		public:
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
			int64_t timestamp_;

			int32_t version_;
		};

		template <typename Header, typename Body, typename Allocator>
		class basic_tcp_protocol<false, Header, Body, Allocator>
			: public basic_protocol<Header, std::add_pointer_t<Body>, Allocator>
		{
		public:
			using base = basic_protocol<Header, std::add_pointer_t<Body>, Allocator>;

			using typename base::header_t;

			constexpr static auto has_request = false;

		public:
			basic_tcp_protocol()
				: base()
				, timestamp_()
				, version_()
				, result_()
			{}

			virtual ~basic_tcp_protocol() = default;

			basic_tcp_protocol(const basic_tcp_protocol&) = default;

			basic_tcp_protocol& operator=(const basic_tcp_protocol&) = default;

			basic_tcp_protocol(basic_tcp_protocol&& other) noexcept
				: base(std::move(other))
				, timestamp_(std::exchange(other.timestamp_, 0))
				, version_(std::exchange(other.version_, 0))
				, result_(std::exchange(other.result_, 0))
			{}

			basic_tcp_protocol& operator=(basic_tcp_protocol&& other) noexcept
			{
				if (std::addressof(other) != this)
				{
					base::operator=(std::move(other));

					timestamp_ = std::exchange(other.timestamp_, 0);
					version_ = std::exchange(other.version_, 0);
					result_ = std::exchange(other.result_, 0);
				}

				return *this;
			}

		public:
			bool operator==(const basic_tcp_protocol& other) const
			{
				return base::operator==(other) && timestamp_ == other.timestamp_ && version_ == other.version_ &&
					   result_ == other.result_;
			}

			std::ostream& operator<<(std::ostream& os) const
			{
				os << this->header() << " " << this->body() << " ";

				os << timestamp() << " " << version() << " " << result();

				return os;
			}

		public:
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

			int32_t result() const
			{
				return result_;
			}

			void result(int32_t value)
			{
				result_ = value;
			}

		private:
			int64_t timestamp_;

			int32_t version_;

			int32_t result_;
		};

	} // namespace virgo
} // namespace aquarius