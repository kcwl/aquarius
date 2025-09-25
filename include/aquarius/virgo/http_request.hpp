#pragma once
#include <aquarius/virgo/basic_http_protocol.hpp>
#include <aquarius/serialize/json.hpp>
#include <string_view>
#include <ranges>
#include <aquarius/serialize/parse_helper.hpp>

namespace aquarius
{
	namespace virgo
	{
		template <detail::string_literal Router, typename Header, typename Body>
		class http_request : public basic_http_protocol<true, Router, Header, Body, std::allocator<Body>>
		{
		public:
			using base = basic_http_protocol<true, Router, Header, Body, std::allocator<Body>>;

			using base::router;

			using base::has_request;

			using typename base::body_t;

			using typename base::header_t;

		public:
			http_request() = default;

		public:
			template <typename T>
			void consume(detail::flex_buffer<T>& buffer, error_code& ec)
			{
				ec = make_error_code(virgo::http_status::ok);

				auto span = std::span<T>(buffer.rdata(), buffer.active());

				auto buf_view = span | std::views::slide(4);

				auto iter = std::ranges::find_if(buf_view, [] (const auto& value)
												{
													if (value.size() < 4)
														return false;

													return std::string_view(value) == "\r\n\r\n";
												});

				if (iter == buf_view.end())
				{
					ec = make_error_code(virgo::http_status::bad_request);
					return;
				}

				auto len = std::distance(buf_view.begin(), iter);

				buffer.consume(len + 4);

				auto buf = span.subspan(0, len);

				auto header_view =  std::views::split(buf, '\n');

				for (const auto header : header_view)
				{
					auto itor = std::ranges::find_if(header.begin(), header.end(), [] (const auto& value) { return value == ':'; });

					if (itor == header.end()) 
					{
						ec = make_error_code(virgo::http_status::bad_request);
						return;
					}

					auto len = std::distance(header.begin(), itor);

					auto key = std::string(std::string_view(header).substr(0, len));

					auto value = std::string(std::string_view(header).substr(len + 1));

					if (value.back() == '\r')
						value = value.substr(0, value.size() - 1);
					
					if (value.front() == ' ')
						value = value.substr(1);

					this->set_field(key, value);
				}

				//if constexpr (!std::same_as<header_t, int>)
				//{
				//	this->header().serialize(buffer);
				//}

				if constexpr (!std::same_as<body_t, int>)
				{
					this->body().serialize(buffer);
				}
			}

			template <typename T>
			void commit(detail::flex_buffer<T>& buffer, error_code& ec)
			{
				std::string str = std::format("{} {} {}\r\n", from_method_string(this->method()), this->path(),
											  from_version_string(this->version()));

				detail::flex_buffer<char> body_buffer{};

				if constexpr (!std::same_as<body_t, int>)
				{
					this->body().deserialize(body_buffer);
				}
				
				if (!body_buffer.empty())
				{
					this->content_length(body_buffer.active());
				}

				for (auto& s : this->fields_)
				{
					str += std::format("{}: {}\r\n", s.first, s.second);
				}

				if (str.size() > buffer.remain())
				{
					ec = make_error_code(http_status::bad_request);
					return;
				}

				std::copy(str.begin(), str.end(), buffer.wdata());
				buffer.commit(str.size());

				//if constexpr (!std::same_as<header_t, int>)
				//{
				//	this->header().deserialize(buffer);
				//}

				std::string end_line = "\r\n";
				std::copy(end_line.begin(), end_line.end(), buffer.wdata());
				buffer.commit(end_line.size());

				if (body_buffer.active() > buffer.remain())
				{
					ec = make_error_code(virgo::http_status::bad_request);
					return;
				}

				buffer.append(std::move(body_buffer));

				ec = make_error_code(virgo::http_status::ok);
			}

		private:
			virgo::json_parse body_parse_;
		};
	} // namespace virgo
} // namespace aquarius