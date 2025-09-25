#pragma once
#include <aquarius/virgo/basic_tcp_protocol.hpp>
#include <aquarius/virgo/http_fields.hpp>
#include <aquarius/virgo/http_version.hpp>
#include <aquarius/virgo/http_method.hpp>
#include <aquarius/virgo/http_status.hpp>

namespace aquarius
{
	namespace virgo
	{
		template <bool Request, detail::string_literal Router, typename Header, typename Body, typename Allocator>
		class basic_http_protocol : public basic_tcp_protocol<Request, Router, Header, Body, Allocator>,
									public http_fields
		{
		public:
			using base = basic_tcp_protocol<Request, Router, Header, Body, Allocator>;

			using base::router;

			using typename base::header_t;

			constexpr static auto has_request = Request;

		public:
			basic_http_protocol() = default;

		public:
			http_method method() const
			{
				return method_;
			}

			void method(http_method method)
			{
				method_ = method;
			}

			http_version version() const
			{
				return static_cast<http_version>(base::version());
			}

			void version(http_version version)
			{
				base::version(static_cast<int32_t>(version));
			}

			auto& get_params()
			{
				return get_params_;
			}

		private:
			http_method method_;

			std::vector<std::pair<std::string, std::string>> get_params_;
		};

		template <detail::string_literal Router, typename Header, typename Body, typename Allocator>
		class basic_http_protocol<false, Router, Header, Body, Allocator>
			: public basic_tcp_protocol<false, Router, Header, Body, Allocator>, public http_fields
		{
		public:
			using base = basic_tcp_protocol<false, Router, Header, Body, Allocator>;

			using base::router;

			using typename base::header_t;

			constexpr static auto has_request = false;

		public:
			basic_http_protocol() = default;

		public:
			std::string_view reason() const
			{
				return reason_;
			}

			void reason(std::string_view r)
			{
				reason_ = r;
			}

			http_version version() const
			{
				return static_cast<http_version>(base::version());
			}

			void version(http_version version)
			{
				base::version(static_cast<int32_t>(version));
			}

		private:
			http_status status_;

			std::string_view reason_;
		};
	} // namespace virgo
} // namespace aquarius