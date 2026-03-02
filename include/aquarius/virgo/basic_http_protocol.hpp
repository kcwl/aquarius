#pragma once
#include <aquarius/basic_protocol.hpp>
#include <aquarius/virgo/http_status.hpp>
#include <aquarius/virgo/http_version.hpp>

namespace aquarius
{
	namespace virgo
	{
		template <bool Request, virgo::http_method Method, typename Header, typename Body,
				  typename Allocator = std::allocator<Body>>
		class basic_http_protocol : public basic_protocol<Header, std::add_pointer_t<Body>, Allocator>
		{
			constexpr static auto crlf = "\r\n"sv;

			using key_t = std::string;

			using value_t = std::string;

			constexpr static auto __seq_number__ = "Seq-Number"sv;
			constexpr static auto __content_length__ = "Content-Length"sv;

		public:
			using base = basic_protocol<Header, std::add_pointer_t<Body>, Allocator>;

			constexpr static auto has_request = Request;

		public:
			basic_http_protocol() = default;

			virtual ~basic_http_protocol() = default;

			basic_http_protocol(const basic_http_protocol& other) = default;

			basic_http_protocol& operator=(const basic_http_protocol& other) = default;

			basic_http_protocol(basic_http_protocol&& other) noexcept = default;

			basic_http_protocol& operator=(basic_http_protocol&& other) noexcept = default;

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

				for (auto& s : fields_)
				{
					headline += std::format("{}: {}\r\n", s.first, s.second);
				}

				return headline;
			}

			void set_header(const std::string& key, const std::string& value)
			{
				set_field(key, value);
			}

			std::string content_type() const
			{
				return find("Content-Type");
			}

			value_t find(const key_t& key) const
			{
				auto iter = fields_.find(key);

				if (iter == fields_.end())
					return value_t{};

				return iter->second;
			}

			void set_field(const key_t& key, const value_t& v)
			{
				fields_[key] = v;
			}

			uint32_t seq_number()
			{
				return to_integer<uint32_t>(find(std::string(__seq_number__)));
			}

			void seq_number(uint32_t v)
			{
				this->set_field(std::string(__seq_number__), std::to_string(v));
			}

			bool has_content_length() const
			{
				return !this->find(std::string(__content_length__)).empty();
			}

			void content_length(uint64_t len)
			{
				this->set_field(std::string(__content_length__), std::to_string(len));
			}

			uint64_t content_length()
			{
				auto value = this->find(std::string(__content_length__));

				if (value.empty())
					return 0;

				return to_integer<uint64_t>(value);
			}

		public:
			virtual bool commit(flex_buffer& buffer) override
			{
				flex_buffer cur_buffer{};
				this->header().serialize(cur_buffer);

				serialize_custom_header(cur_buffer);

				this->body().serialize(cur_buffer);

				auto headline = commit_header();

				headline += this->header_field();

				headline += crlf;

				buffer.sputn(headline.data(), headline.size());

				if constexpr (has_request && Method != virgo::http_method::get)
				{
					return true;
				}

				if (cur_buffer.size() < 3)
				{
					return true;
				}

				buffer.sputn((char*)cur_buffer.data().data(), cur_buffer.data().size());

				return true;
			}

			virtual bool consume_header(flex_buffer& buffer) override
			{
				error_code ec{};
				parse_field(std::span<char>((char*)buffer.data().data(), buffer.size()), ec);

				auto buf = deserialize_custom_header();

				this->header().deserialize(buf);

				return !ec;
			}

		protected:
			virtual std::string commit_header() = 0;

		private:
			void serialize_custom_header(flex_buffer& value)
			{
				this->set_field("Aquarius-Header", std::string((char*)value.data().data(), value.size()));

				value.consume(value.size());
			}

			flex_buffer deserialize_custom_header() const
			{
				flex_buffer buffer{};

				auto value = this->find("Aquarius-Header");

				buffer.sputn(value.data(), value.size());

				return std::move(buffer);
			}

			void parse_field(std::span<char> buffer, error_code& ec)
			{
				ec = error_code{};

				auto headers = buffer | std::views::split(crlf);

				for (const auto header : headers)
				{
					auto str = std::string_view(header);

					if (str == crlf)
						break;

					auto pos = str.find(":");

					if (pos == std::string_view::npos)
					{
						ec = boost::asio::error::eof;

						break;
					}

					auto key = str.substr(0, pos);
					auto value = str.substr(pos + 1, str.size() - pos - 1);

					this->set_header(std::string(key), std::string(value));
				}
			}

		private:
			template <typename T>
			T to_integer(const std::string& value)
			{
				std::stringstream ss{};
				ss << value;

				T result;
				ss >> result;

				return result;
			}

		protected:
			std::map<std::string, std::string> fields_;
		};
	} // namespace virgo
} // namespace aquarius