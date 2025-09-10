#pragma once
#include <map>
#include <aquarius/virgo/basic_protocol_message.hpp>
#include <aquarius/detail/flex_buffer.hpp>
#include <aquarius/virgo/parse_helper.hpp>
#include <aquarius/detail/json.hpp>
#include <aquarius/virgo/http_header.hpp>
#include <aquarius/error_code.hpp>
#include <expected>

namespace aquarius
{
	namespace virgo
	{
		template <bool Request, typename Header, typename Body>
		class basic_http_message : public basic_protocol_message<Request, Header,http_header ,Body, detail::json_parse>
		{
			using base = basic_protocol_message<Request, Header, http_header, Body, detail::json_parse>;

		public:
			basic_http_message() = default;

		public:
			template <typename T>
			std::expected<bool, error_code> commit(detail::flex_buffer<T>& buffer)
			{
				auto result = base::commit(buffer);

				if (!result.has_value())
				{
					return result;
				}

				std::string str{};

				for (auto& s : fields_)
				{
					str += std::format("{}: {}\r\n", s.first, s.second);
				}

				if (str.size() > buffer.remain())
					return std::unexpected(make_error_code(http_status::bad_request));

				std::copy(str.begin(), str.end(), buffer.wdata());

				buffer.commit(str.size());

				this->parse_.to_datas(this->header(), buffer);

				if (!result.has_value())
				{
					return result;
				}

				std::string end_line = "\r\n";

				std::copy(end_line.begin(), end_line.end(), buffer.wdata());
				buffer.commit(end_line.size());

				this->parse_.to_datas(this->body(), buffer);

				return true;
			}

			template <typename T>
			std::expected<bool, error_code> consume(detail::flex_buffer<T>& buffer)
			{
				std::expected<bool, error_code> result;

				while (!buffer.empty())
				{
					result = read_key<T, ':'>(buffer).and_then(
						[&](const auto& key) -> std::expected<bool, error_code>
						{
							if (key.empty())
								return std::unexpected(make_error_code(http_status::ok));

							return read_value<T, '\r'>(buffer).and_then(
								[&](const auto& value) -> std::expected<bool, error_code>
								{
									set_field(key, value);

									buffer.consume(1);

									return true;
								});
						});

					if (!result.has_value())
						break;
				}

				return result;
			}

		public:
			std::ostream& operator<<(std::ostream& os) const
			{
				for (auto& s : fields_)
				{
					os << s.first << ":" << s.second << "\r\n";
				}

				os << "\r\n";

				return os;
			}

		public:
			std::string find(const std::string& f) const
			{
				auto iter = fields_.find(f);

				if (iter == fields_.end())
					return {};

				return iter->second;
			}

			template <typename T>
			void set_field(const std::string& f, T v)
			{
				std::stringstream ss;
				ss << v;
				fields_[f] = ss.str();
			}

			bool has_content_length() const
			{
				return find("Content-Length").empty();
			}

			void content_length(uint64_t len)
			{
				fields_["Content-Length"] = std::to_string(len);
			}

			uint64_t content_length()
			{
				auto iter = fields_.find("Content-Length");
				if (iter == fields_.end())
					return 0;

				return std::atoi(iter->second.data());
			}

			bool keep_alive()
			{
				auto iter = fields_.find("Connection");
				if (iter == fields_.end())
					return false;

				return iter->second == "keep-alive" ? true : false;
			}

			void keep_alive(bool k)
			{
				fields_["Connection"] = k ? "keep-alive" : "close";
			}

		private:
			std::map<std::string, std::string> fields_;
		};

		template <bool Request, typename Body, typename Allocator>
		inline std::ostream& operator<<(std::ostream& os, const basic_http_message<Request, Body, Allocator>& other)
		{
			other << os;

			return os;
		}
	} // namespace virgo
} // namespace aquarius