#pragma once
#include <aquarius/virgo/basic_protocol.hpp>

namespace aquarius
{
	namespace virgo
	{
		template <bool Request, const std::string_view& Router, typename Header, typename Body, typename Allocator>
		class basic_tcp_protocol : public basic_protocol<Router, std::add_pointer_t<Body>, Allocator>
		{
		public:
			using base = basic_protocol<Router, std::add_pointer_t<Body>, Allocator>;

			using base::router;

			using header_t = Header;

			using typename base::body_t;

			constexpr static auto hass_request = Request;

		public:
			basic_tcp_protocol() = default;

		public:
			header_t& header()
			{
				return header_;
			}

			const header_t& header() const
			{
				return header_;
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
			header_t header_;

			int64_t timestamp_;

			int32_t version_;
		};

		template <const std::string_view& Router, typename Header, typename Body, typename Allocator>
		class basic_tcp_protocol<false, Router, Header, Body, Allocator> : public basic_protocol<Router, std::add_pointer_t<Body>, Allocator>
		{
		public:
			using base = basic_protocol<Router, std::add_pointer_t<Body>, Allocator>;

			using base::router;

			using header_t = Header;

			constexpr static auto hass_request = false;

		public:
			basic_tcp_protocol() = default;

		public:
			header_t& header()
			{
				return header_;
			}

			const header_t& header() const
			{
				return header_;
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

			int32_t result() const
			{
				return result_;
			}

			void result(int32_t value)
			{
				result_ = value;
			}

		private:
			header_t header_;

			int64_t timestamp_;

			int32_t version_;

			int32_t result_;
		};
	} // namespace virgo
} // namespace aquarius