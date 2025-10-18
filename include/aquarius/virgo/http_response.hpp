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
			void commit(flex_buffer<T>& buffer)
			{
				std::string str = std::format("{} {} {}\r\n", from_version_string(this->version()),
											  std::to_string(this->result()), this->reason());
				flex_buffer<char> body_buffer{};

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

				buffer.put(str.begin(), str.end());

				this->header().serialize(buffer);

				constexpr std::string_view end_line = "\r\n\r\n"sv;

				buffer.put(end_line.begin(), end_line.end());

				buffer.append(std::move(body_buffer));
			}

			template <typename T>
			void consume(flex_buffer<T>& buffer)
			{
				this->header().serialize(buffer);
				
				this->body().serialize(buffer);
			}

		private:
			serialize::json_parse body_parse_;
		};
	} // namespace virgo
} // namespace aquarius