#pragma once
#include <aquarius/virgo/basic_http_protocol.hpp>
#include <aquarius/virgo/serialize/json.hpp>
#include <aquarius/virgo/http_method.hpp>
#include <aquarius/virgo/http_status.hpp>
#include <aquarius/virgo/http_version.hpp>

namespace aquarius
{
	namespace virgo
	{

		template <detail::string_literal Router, typename Header, typename Body>
		class http_response : public basic_http_protocol<false, Router, Header, Body, std::allocator<Body>>
		{
		public:
			using base = basic_http_protocol<false, Router, Header, Body, std::allocator<Body>>;

			using base::router;

			using base::has_request;

			using typename base::body_t;

			using typename base::header_t;

		public:
			http_response() = default;

		public:
			template <typename T>
			std::expected<bool, error_code> commit(detail::flex_buffer<T>& buffer)
			{
				std::string str = std::format("{} {} {}\r\n", from_version_string(this->version()),
											  from_status_string(this->status()), this->reason());

				detail::flex_buffer<T> body_buffer{};
				this->body_parse_.to_datas(this->body(), body_buffer);

				this->content_length(body_buffer.active());

				for (auto& s : this->fields_)
				{
					str += std::format("{}: {}\r\n", s.first, s.second);
				}

				if (str.size() > buffer.remain())
					return std::unexpected(make_error_code(http_status::bad_request));

				std::copy(str.begin(), str.end(), buffer.wdata());

				buffer.commit(str.size());

				//this->header_parse_.to_datas(this->header());
				if constexpr (!std::same_as<header_t, int>)
				{
					this->header().serialize(buffer);
				}

				std::string end_line = "\r\n";

				std::copy(end_line.begin(), end_line.end(), buffer.wdata());
				buffer.commit(end_line.size());

				this->body_parse_.to_datas(this->body(), buffer);

				return true;
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

				//this->body() = body_parse_.from_datas<body_t>(buffer);
				this->body().deserialize(buffer);
			}

		private:
			virgo::json_parse body_parse_;
		};
	} // namespace virgo
} // namespace aquarius