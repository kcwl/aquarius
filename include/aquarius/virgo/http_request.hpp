#pragma once
#include <aquarius/virgo/basic_http_protocol.hpp>
#include <aquarius/serialize/json.hpp>
#include <aquarius/ip/concept.hpp>

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
			bool operator==(const http_request& other) const
			{
				return base::operator==(other);
			}

			std::ostream& operator<<(std::ostream& os) const
			{
				return base::operator<<(os);
			}

		public:
			template <typename T>
			void consume(flex_buffer<T>& buffer)
			{
				auto sp = std::span<T>(buffer.rdata(), buffer.active());

				auto fourth_view = sp | std::views::slide(4);

				auto iter = std::ranges::find_if(fourth_view, [] (const auto& value)
									 {
										 if (value.size() < 4)
											 return false;

										 return std::string_view(value) == "\r\n\r\n";
									 });

				if (iter == fourth_view.end())
					return;

				auto len = std::distance(fourth_view.begin(), iter);

				flex_buffer<T> header_buffer(buffer.rdata(), len);

				this->header().deserialize(header_buffer);

				buffer.consume(4 + len);

				flex_buffer<T> body_buffer(buffer.rdata(), buffer.active());

				this->body().deserialize(body_buffer);
			}

			template <typename T>
			void commit(flex_buffer<T>& buffer)
			{
				flex_buffer<char> body_buffer{};

				this->header().serialize(body_buffer);

				constexpr auto sp = "\r\n\r\n"sv;

				body_buffer.put(sp.begin(), sp.end());

				this->body().serialize(body_buffer);

				this->content_length(body_buffer.active());

				buffer.append(std::move(body_buffer));
			}

		private:
			serialize::json_parse body_parse_;
		};

		template <detail::string_literal Router, typename Header, typename Body>
		std::ostream& operator<<(std::ostream& os, const http_request<Router, Header, Body>& req)
		{
			req << os;

			return os;
		}

		template<detail::string_literal Router, typename Header, typename Body>
		struct is_message_type<http_request<Router, Header, Body>> : std::true_type {};
	} // namespace virgo
} // namespace aquarius