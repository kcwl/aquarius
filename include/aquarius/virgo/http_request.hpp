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
		class http_request : public basic_http_protocol<true, Header, Body>
		{
		public:
			using base = basic_http_protocol<true, Header, Body>;

			using base::has_request;

			using base::json_type;

			using base::splitor;

			constexpr static auto router = detail::bind_param<Router>::value;

		public:
			http_request() = default;
			virtual ~http_request() = default;

			http_request(const http_request&) = default;
			http_request& operator=(const http_request&) = default;

			http_request(http_request&&) noexcept = default;
			http_request& operator=(http_request&&) noexcept = default;

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
					auto sp = std::span<char>((char*)buffer.data().data(), buffer.data().size());

					auto third_view = sp | std::views::slide(3);

					auto iter = std::ranges::find_if(third_view,
													 [](const auto& value)
													 {
														 if (value.size() < 3)
															 return false;

														 return std::string_view(value) == splitor;
													 });

					if (iter == third_view.end())
						return;

					auto len = std::distance(third_view.begin(), iter);

					this->header().deserialize(buffer);
				}

				this->body().deserialize(buffer);
			}

			void commit(flex_buffer& buffer)
			{
				flex_buffer body_buffer{};

				this->body().serialize(body_buffer);

				this->content_length(body_buffer.size());

				auto type = this->find("Content-Type");

				if (type == json_type)
				{
					this->header().serialize(buffer);

					buffer.sputn(splitor.data(), splitor.size());
				}

				buffer.sputn(body_buffer.data().data(), body_buffer.data().size());
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