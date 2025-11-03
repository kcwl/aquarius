#pragma once
#include <aquarius/virgo/basic_http_protocol.hpp>
#include <aquarius/serialize/json.hpp>
#include <aquarius/ip/concept.hpp>
#include <ranges>

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

			using base::json_type;

			using base::splitor;

			using typename base::body_t;

			using typename base::header_t;

		public:
			http_request()
				: base()
			{
			}

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
			void consume(flex_buffer& buffer)
			{
				auto type = this->find("Content-Type");

				if (type == json_type)
				{
					auto sp = std::span<char>((char*)buffer.rdata(), buffer.active());

					auto third_view = sp | std::views::slide(3);

					auto iter = std::ranges::find_if(third_view,
													 [] (const auto& value)
													 {
														 if (value.size() < 3)
															 return false;

														 return std::string_view(value) == "<->"sv;
													 });

					if (iter == third_view.end())
						return;

					auto len = std::distance(third_view.begin(), iter);

					flex_buffer header_buffer(buffer.rdata(), len);

					this->header().deserialize(header_buffer);

					buffer.consume(3 + len);
				}

				flex_buffer body_buffer(buffer.rdata(), buffer.active());

				this->body().deserialize(body_buffer);
			}

			void commit(flex_buffer& buffer)
			{
				flex_buffer body_buffer{};

				this->body().serialize(body_buffer);

				this->content_length(body_buffer.active());

				auto type = this->find("Content-Type");

				if (type == json_type)
				{
					this->header().serialize(buffer);

					buffer.put(splitor.begin(), splitor.end());
				}

				buffer.append(std::move(body_buffer));
			}
		};

		template <detail::string_literal Router, typename Header, typename Body>
		std::ostream& operator<<(std::ostream& os, const http_request<Router, Header, Body>& req)
		{
			req << os;

			return os;
		}
	} // namespace virgo

	template <detail::string_literal Router, typename Header, typename Body>
	struct is_message_type<virgo::http_request<Router, Header, Body>> : std::true_type
	{};
} // namespace aquarius