#pragma once
#include <aquarius/basic_protocol.hpp>
#include <aquarius/virgo/http_fields.hpp>
#include <aquarius/virgo/http_version.hpp>
#include <aquarius/virgo/http_method.hpp>
#include <aquarius/virgo/http_status.hpp>

namespace aquarius
{
	namespace virgo
	{
		template <bool Request, detail::string_literal Router, typename Header, typename Body, typename Allocator>
		class basic_http_protocol : public basic_protocol<Router, Header, std::add_pointer_t<Body>, Allocator>,
									public http_fields
		{
		public:
			using base = basic_protocol<Router, Header, std::add_pointer_t<Body>, Allocator>;

			using base::router;

			using typename base::header_t;

			constexpr static auto has_request = Request;

		public:
			basic_http_protocol()
				: method_()
				, version_(http_version::http1_1)
				, get_params_()
			{}

		public:
			bool operator==(const basic_http_protocol& other) const
			{
				return base::operator==(other) && method_ == other.method_ && version_ == other.version_ &&
					   get_params_ == other.get_params_;
			}

			std::ostream& operator<<(std::ostream& os) const
			{
				base::operator<<(os);

				os << method_ << version_;

				for (auto& p : get_params_)
				{
					os << "[" << p.first << ", " << p.second << "]";
				}

				return os;
			}

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
				return version_;
			}

			void version(http_version version)
			{
				version_ = version;
			}

			auto& get_params()
			{
				return get_params_;
			}

			auto fields() const
			{
				return this->fields_;
			}

		private:
			http_method method_;

			http_version version_;

			std::vector<std::pair<std::string, std::string>> get_params_;
		};

		template <detail::string_literal Router, typename Header, typename Body, typename Allocator>
		class basic_http_protocol<false, Router, Header, Body, Allocator>
			: public basic_protocol<Router, Header, std::add_pointer_t<Body>, Allocator>, public http_fields
		{
		public:
			using base = basic_protocol<Router, Header, std::add_pointer_t<Body>, Allocator>;

			using base::router;

			using typename base::header_t;

			constexpr static auto has_request = false;

		public:
			basic_http_protocol()
				: status_()
				, version_()
				, reason_()
			{}

		public:
			bool operator==(const basic_http_protocol& other) const
			{
				return base::operator==(other) && reason_ == other.reason_ && status_ == other.status_ &&
					   version_ == other.version_;
			}

			std::ostream& operator<<(std::ostream& os) const
			{
				base::operator<<(os);

				os << status_ << version_ << reason_;

				return os;
			}

		public:
			std::string_view reason() const
			{
				return reason_;
			}

			void reason(std::string_view r)
			{
				reason_ = r;
			}

			http_status status() const
			{
				return status_;
			}

			void status(http_status s)
			{
				status_ = s;
			}

			http_version version() const
			{
				return version_;
			}

			void version(http_version version)
			{
				version_ = version;
			}

		private:
			http_status status_;

			http_version version_;

			std::string_view reason_;
		};
	} // namespace virgo
} // namespace aquarius