#pragma once
#include <aquarius/basic_protocol.hpp>

namespace aquarius
{
	namespace virgo
	{
		template <bool Request, detail::string_literal Router, typename Header, typename Body, typename Allocator>
		class basic_tcp_protocol : public basic_protocol<Router, Header, std::add_pointer_t<Body>, Allocator>
		{
		public:
			using base = basic_protocol<Router, Header, std::add_pointer_t<Body>, Allocator>;

			using base::router;

			using typename base::header_t;

			using typename base::body_t;

			constexpr static auto has_request = Request;

		public:
			basic_tcp_protocol()
				: timestamp_()
				, version_()
			{}

			virtual ~basic_tcp_protocol() = default;

		public:
			bool operator==(const basic_tcp_protocol& other) const
			{
				return base::operator==(other) && timestamp_ == other.timestamp_ &&
					   version_ == other.version_;
			}

			std::ostream& operator<<(std::ostream& os) const
			{
				base::operator<<(os);

				os << timestamp_ << version_;

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

		template <detail::string_literal Router, typename Header, typename Body, typename Allocator>
		class basic_tcp_protocol<false, Router, Header, Body, Allocator>
			: public basic_protocol<Router, Header, std::add_pointer_t<Body>, Allocator>
		{
		public:
			using base = basic_protocol<Router, Header, std::add_pointer_t<Body>, Allocator>;

			using base::router;

			using typename base::header_t;

			constexpr static auto has_request = false;

		public:
			basic_tcp_protocol()
				: timestamp_()
				, version_()
				, result_()
			{}

		public:
			bool operator==(const basic_tcp_protocol& other) const
			{
				return base::operator==(other) && timestamp_ == other.timestamp_ &&
						   version_ == other.version_ && result_ == other.result_;
			}

			std::ostream& operator<<(std::ostream& os) const
			{
				base::operator<<(os);

				os << timestamp_ << version_ << result_;

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