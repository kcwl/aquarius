#pragma once
#include <aquarius/virgo/basic_http_protocol.hpp>
#include <aquarius/serialize/json.hpp>
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
			void commit(detail::flex_buffer<T>& buffer, error_code& ec)
			{
				ec = make_error_code(virgo::http_status::ok);

				std::string str = std::format("{} {} {}\r\n", from_version_string(this->version()),
											  std::to_string(this->result()), this->reason());
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
				//	this->header().serialize(buffer);
				//}

				std::string end_line = "\r\n";
				std::copy(end_line.begin(), end_line.end(), buffer.wdata());
				buffer.commit(end_line.size());

				if (body_buffer.active() > buffer.remain())
				{
					ec = make_error_code(virgo::http_status::bad_request);
					return;
				}

				if (!body_buffer.empty())
				{
					buffer.append(std::move(body_buffer));
				}
				
				ec = make_error_code(virgo::http_status::ok);
			}

			template <typename T>
			void consume(detail::flex_buffer<T>& buffer, error_code& ec)
			{
				ec = make_error_code(virgo::http_status::ok);

				//const auto sp = std::span<char>(buffer.rdata(), buffer.active());

				//auto parts = sp | std::views::slide(4);

				//auto iter = std::ranges::find_if(parts, [] (auto value)
				//								 {
				//									 if (value.size() < 4)
				//										 return false;

				//									 return std::string_view(value) == "\r\n\r\n";
				//								 });

				//auto len = std::distance(parts.begin(), iter);

				//buffer.consume(len + 4);

				//for (const auto& p : std::views::split(sp.subspan(0, len), "\r\n"))
				//{
				//	auto line = p | std::views::split(':');
				//	if (std::distance(line.begin(),line.end()) < 2)
				//	{
				//		ec = make_error_code(virgo::http_status::bad_request);
				//		break;
				//	}
				//	auto next = line.begin();

				//	std::advance(next, 1);

				//	this->set_field(std::string(std::string_view(*line.begin())), std::string(std::string_view(*next)));
				//}

				//if (this->content_length() > buffer.active())
				//{
				//	ec = make_error_code(virgo::http_status::bad_request);
				//	return;
				//}

				//if constexpr (!std::same_as<header_t, int>)
				//{
				//	this->header().serialize(buffer);
				//}
				
				if constexpr (!std::same_as<body_t, int>)
				{
					this->body().serialize(buffer);
				}
			}

		private:
			serialize::json_parse body_parse_;
		};
	} // namespace virgo
} // namespace aquarius