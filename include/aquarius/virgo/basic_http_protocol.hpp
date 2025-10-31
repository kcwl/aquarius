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
		template <bool Request, virgo::http_method Method, detail::string_literal Router, typename Header,
				  typename Body, typename Allocator>
		class basic_http_protocol : public basic_protocol<Router, Header, std::add_pointer_t<Body>, Allocator>,
									public http_fields
		{
		public:
			constexpr static auto json_type = "aquarius-json"sv;

			constexpr static auto splitor = "<->"sv;

			using base = basic_protocol<Router, Header, std::add_pointer_t<Body>, Allocator>;

			using base::router;

			constexpr static auto method = Method;

			using typename base::header_t;

			constexpr static auto has_request = Request;

		public:
			basic_http_protocol()
				: base()
				, version_(http_version::http1_1)
				, get_params_()
			{}

		public:
			bool operator==(const basic_http_protocol& other) const
			{
				return base::operator==(other) && version_ == other.version_ &&
					   get_params_ == other.get_params_;
			}

			std::ostream& operator<<(std::ostream& os) const
			{
				base::operator<<(os);

				os << method << version_;

				for (auto& p : get_params_)
				{
					os << "[" << p.first << ", " << p.second << "]";
				}

				return os;
			}

		public:
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
			http_version version_;

			std::vector<std::pair<std::string, std::string>> get_params_;
		};

		template <virgo::http_method Method, detail::string_literal Router, typename Header, typename Body,
				  typename Allocator>
		class basic_http_protocol<false, Method, Router, Header, Body, Allocator>
			: public basic_protocol<Router, Header, std::add_pointer_t<Body>, Allocator>, public http_fields
		{
		public:
			constexpr static auto json_type = "aquarius-json"sv;

			constexpr static auto splitor = "<->"sv;

			using base = basic_protocol<Router, Header, std::add_pointer_t<Body>, Allocator>;

			using base::router;

			constexpr static auto method = Method;

			using typename base::header_t;

			constexpr static auto has_request = false;

		public:
			basic_http_protocol()
				: base()
				, status_()
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

			http_status result() const
			{
				return status_;
			}

			void result(int s)
			{
				status_ = static_cast<http_status>(s);
			}

			http_version version() const
			{
				return version_;
			}

			void version(http_version version)
			{
				version_ = version;
			}

			auto fields() const
			{
				return this->fields_;
			}

		private:
			http_status status_;

			http_version version_;

			std::string_view reason_;
		};
	} // namespace virgo
} // namespace aquarius