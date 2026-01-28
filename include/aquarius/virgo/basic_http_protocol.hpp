#pragma once
#include <aquarius/basic_protocol.hpp>
#include <aquarius/virgo/http_status.hpp>
#include <aquarius/virgo/http_version.hpp>

namespace aquarius
{
	namespace virgo
	{
		template <bool Request, typename Header, typename Body, typename Allocator = std::allocator<Body>>
		class basic_http_protocol : public basic_protocol<Header, std::add_pointer_t<Body>, Allocator>
		{
		public:
			constexpr static auto json_type = "aquarius-json"sv;

			constexpr static auto splitor = "<->"sv;

			using base = basic_protocol<Header, std::add_pointer_t<Body>, Allocator>;

			constexpr static auto has_request = Request;

		public:
			basic_http_protocol()
				: basic_http_protocol(header_fields{})
			{}

			basic_http_protocol(header_fields f)
				: base(f)
				, version_(http_version::unknown)
			{}

			virtual ~basic_http_protocol() = default;

			basic_http_protocol(const basic_http_protocol& other)
				: base(other)
				, version_(other.version_)
			{}

			basic_http_protocol& operator=(const basic_http_protocol& other)
			{
				if (std::addressof(other) != this)
				{
					base::operator=(other);
					header_fields::operator=(other);
					version_ = other.version_;
				}

				return *this;
			}

			basic_http_protocol(basic_http_protocol&& other) noexcept
				: base(std::move(other))
				, header_fields(std::move(other))
				, version_(std::exchange(other.version_, http_version::unknown))
			{}

			basic_http_protocol& operator=(basic_http_protocol&& other) noexcept
			{
				if (std::addressof(other) != this)
				{
					base::operator=(std::move(other));
					header_fields::operator=(std::move(other));
					version_ = std::exchange(other.version_, http_version::unknown);
				}

				return *this;
			}

		public:
			bool operator==(const basic_http_protocol& other) const
			{
				return base::operator==(other) && version_ == other.version_;
			}

			std::ostream& operator<<(std::ostream& os) const
			{
				os << this->header() << " " << this->body() << " " << static_cast<int>(version());

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

			bool has_content_length() const
			{
				return !this->find("Content-Length").empty();
			}

			void content_length(uint64_t len)
			{
				this->set_field("Content-Length", std::to_string(len));
			}

			uint64_t content_length()
			{
				auto value = this->find("Content-Length");

				if (value.empty())
					return 0;

				std::stringstream ss{};
				ss << value;

				uint64_t result;
				ss >> result;

				return result;
			}

			bool keep_alive()
			{
				auto value = this->find("Connection");

				return value == "keep-alive" ? true : false;
			}

			void keep_alive(bool k)
			{
				this->set_field("Connection", k ? "keep-alive" : "close");
			}

		private:
			http_version version_;
		};

		template <typename Header, typename Body, typename Allocator>
		class basic_http_protocol<false, Header, Body, Allocator>
			: public basic_protocol<Header, std::add_pointer_t<Body>, Allocator>
		{
		public:
			constexpr static auto json_type = "aquarius-json"sv;

			constexpr static auto splitor = "<->"sv;

			using base = basic_protocol<Header, std::add_pointer_t<Body>, Allocator>;

			constexpr static auto has_request = false;

		public:
			basic_http_protocol()
				: basic_http_protocol(header_fields{})
			{}

			basic_http_protocol(header_fields f)
				: base(std::move(f))
				, status_(virgo::http_status::ok)
				, version_(virgo::http_version::unknown)
			{}

			virtual ~basic_http_protocol() = default;

			basic_http_protocol(const basic_http_protocol& other)
				: base(other)
				, status_(other.status_)
				, version_(other.version_)
			{}

			basic_http_protocol& operator=(const basic_http_protocol& other)
			{
				if (this != std::addressof(other))
				{
					base::operator=(other);
					header_fields::operator=(other);
					status_ = other.status_;
					version_ = other.version_;
				}

				return *this;
			}

			basic_http_protocol(basic_http_protocol&& other) noexcept
				: base(std::move(other))
				, status_(std::exchange(other.status_, virgo::http_status::ok))
				, version_(std::exchange(other.version_, virgo::http_version::unknown))
			{}

			basic_http_protocol& operator=(basic_http_protocol&& other) noexcept
			{
				if (this != std::addressof(other))
				{
					base::operator=(std::move(other));
					status_ = std::exchange(other.status_, virgo::http_status::ok);
					version_ = std::exchange(other.version_, 0);
				}

				return *this;
			}

		public:
			bool operator==(const basic_http_protocol& other) const
			{
				return base::operator==(other) && status_ == other.status_ && version_ == other.version_;
			}

			std::ostream& operator<<(std::ostream& os) const
			{
				return os << this->header() << " " << this->body() << " " << result() << " " << version();
			}

		public:
			http_status result() const
			{
				return status_;
			}

			void result(int s)
			{
				return result(static_cast<http_status>(s));
			}

			void result(http_status s)
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

			bool has_content_length() const
			{
				return !this->find("Content-Length").empty();
			}

			void content_length(uint64_t len)
			{
				this->set_field("Content-Length", std::to_string(len));
			}

			uint64_t content_length()
			{
				auto value = this->find("Content-Length");

				if (value.empty())
					return 0;

				std::stringstream ss{};
				ss << value;

				uint64_t result;
				ss >> result;

				return result;
			}

			bool keep_alive()
			{
				auto value = this->find("Connection");

				return value == "keep-alive" ? true : false;
			}

			void keep_alive(bool k)
			{
				this->set_field("Connection", k ? "keep-alive" : "close");
			}

		private:
			http_status status_;

			http_version version_;
		};
	} // namespace virgo
} // namespace aquarius