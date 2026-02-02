#pragma once
#include <aquarius/basic_protocol.hpp>
#include <aquarius/virgo/http_status.hpp>
#include <aquarius/virgo/http_version.hpp>

namespace aquarius
{
	namespace virgo
	{
		template <bool Request, typename Header, typename Body, typename Allocator = std::allocator<Body>>
		class basic_http_protocol : public basic_protocol<Header, std::add_pointer_t<Body>, Allocator>,
									public header_fields
		{
		public:
			using base = basic_protocol<Header, std::add_pointer_t<Body>, Allocator>;

			using header_field_type = header_fields;

			constexpr static auto has_request = Request;

		public:
			basic_http_protocol() = default;

			virtual ~basic_http_protocol() = default;

			basic_http_protocol(const basic_http_protocol& other) = default;

			basic_http_protocol& operator=(const basic_http_protocol& other) = default;

			basic_http_protocol(basic_http_protocol&& other) noexcept = default;

			basic_http_protocol& operator=(basic_http_protocol&& other) noexcept = default;

		public:
			bool operator==(const basic_http_protocol& other) const
			{
				return base::operator==(other) && header_field_type::operator==(other);
			}

			std::ostream& operator<<(std::ostream& os) const
			{
				os << this->header() << " " << this->body();

				return os;
			}

		public:
			bool keep_alive()
			{
				auto value = this->find("Connection");

				return value == "keep-alive" ? true : false;
			}

			void keep_alive(bool k)
			{
				this->set_field("Connection", k ? "keep-alive" : "close");
			}

			std::string header_field() const
			{
				std::string headline{};

				for (auto& s : this->fields_)
				{
					headline += std::format("{}: {}\r\n", s.first, s.second);
				}

				return headline;
			}

			void set_header(const std::string& key, const std::string& value)
			{
				this->set_field(key, value);
			}

			std::string content_type() const
			{
				return this->find("Content-Type");
			}

		protected:
			void serialize_custom_header(flex_buffer& value)
			{
				this->set_field("Set-Cookie", std::string((char*)value.data().data(), value.size()));
			}

			flex_buffer deserialize_custom_header() const
			{
				flex_buffer buffer{};

				auto value = find("Set-Cookie");

				buffer.sputn(value.data(), value.size());

				return std::move(buffer);
			}
		};
	} // namespace virgo
} // namespace aquarius