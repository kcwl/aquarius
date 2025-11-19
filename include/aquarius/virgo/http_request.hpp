#pragma once
#include <aquarius/virgo/basic_http_protocol.hpp>
#include <aquarius/serialize/json.hpp>
#include <aquarius/ip/concept.hpp>
#include <ranges>

namespace aquarius
{
	namespace virgo
	{
		template <detail::string_literal Router, virgo::http_method Method, typename Header, typename Body>
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
				this->body().deserialize(buffer);
			}

			void commit(flex_buffer& buffer)
			{
				flex_buffer body_buffer{};

				std::string headline{};

				if (Method == virgo::http_method::get)
				{
					flex_buffer tempget;
					this->body().serialize(tempget);
					std::string temp_str;
					tempget.get(temp_str);
					headline = std::format("{} /{}{} {}\r\n", virgo::from_method_string(method), Message::router,
										   temp_str, virgo::from_string_version(msg.version()));
				}
				else
				{
					this->body().serialize(body_buffer);

					this->content_length(body_buffer.size());

					headline =
						std::format("{} {} {}\r\n", virgo::from_string_version(resp.version()),
									static_cast<int>(resp.result()), virgo::from_status_string(resp.result()).data());
				}

				for (auto& s : resp.fields())
				{
					headline += std::format("{}: {}\r\n", s.first, s.second);
				}

				headline += "\r\n";

				buffer.sputn(headline.c_str(), headline.size());

				if (temp.size() != 0)
				{
					buffer.sputn((char*)temp.data().data(), temp.data().size());
				}
			}
		};

		template <detail::string_literal Router, virgo::http_method Method, typename Header, typename Body>
		std::ostream& operator<<(std::ostream& os, const http_request<Router, Method, Header, Body>& req)
		{
			req << os;

			return os;
		}
	} // namespace virgo

	template <detail::string_literal Router, virgo::http_method Method, typename Header, typename Body>
	struct is_message_type<virgo::http_request<Router, Method, Header, Body>> : std::true_type
	{};
} // namespace aquarius