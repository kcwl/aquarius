#pragma once
#include <aquarius/virgo/basic_http_protocol.hpp>
#include <aquarius/detail/string_parse.hpp>
#include <aquarius/detail/json.hpp>
#include <string_view>

namespace aquarius
{
	namespace virgo
	{
		template <const std::string_view& Router, typename Header, typename Body>
		class http_request : public basic_http_protocol<true, Router, Header, Body, std::allocator<Body>>
		{
			using base = basic_http_protocol<true, Router, Header, Body, std::allocator<Body>>;

			using base::router;

			using base::has_request;

		public:
			http_request() = default;

		public:
			template <typename T>
			bool commit(detail::flex_buffer<T>& buffer)
			{
				std::string str = std::format("{} {} {}\r\n", from_method_string(this->method()), this->path(),
											  from_version_string(this->version()));

				auto body_str = this->header_parse_.to_datas(this->body());

				if (!body_str.empty())
				{
					this->content_length(body_str.size());
				}

				for (auto& s : this->fields_)
				{
					str += std::format("{}: {}\r\n", s.first, s.second);
				}

				if (str.size() > buffer.remain())
					return std::unexpected(make_error_code(http_status::bad_request));

				std::copy(str.begin(), str.end(), buffer.wdata());

				buffer.commit(str.size());

				if (!this->header_parse_.to_datas(this->header(), buffer))
					return false;

				std::string end_line = "\r\n";

				std::copy(end_line.begin(), end_line.end(), buffer.wdata());
				buffer.commit(end_line.size());

				this->body_parse_.to_datas(this->body(), buffer);
			}

			template <typename T>
			void consume(detail::flex_buffer<T>& buffer)
			{
				while (!buffer.empty())
				{
					auto result = read_key<T, ':'>(buffer).and_then(
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

				body_parse_.from_datas(this->body(), buffer);
			}

		private:
			detail::string_parse header_parse_;

			detail::json_parse body_parse_;
		};
	} // namespace virgo
} // namespace aquarius