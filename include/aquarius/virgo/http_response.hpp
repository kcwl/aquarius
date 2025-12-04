#pragma once
#include <aquarius/virgo/basic_http_protocol.hpp>
#include <aquarius/serialize/json.hpp>
#include <aquarius/virgo/http_method.hpp>
#include <aquarius/virgo/http_status.hpp>
#include <aquarius/virgo/http_version.hpp>
#include <aquarius/ip/concept.hpp>

namespace aquarius
{
	namespace virgo
	{
		template <virgo::http_method Method, typename Header, typename Body>
		class http_response : public basic_http_protocol<false, Header, Body>
		{
		public:
			using base = basic_http_protocol<false, Header, Body>;

			using base::has_request;

			using base::json_type;

			using base::splitor;

		public:
			http_response() = default;
			virtual ~http_response() = default;

			http_response(const http_response&) = delete;
			http_response& operator=(const http_response&) = delete;

			http_response(http_response&&) noexcept = default;
			http_response& operator=(http_response&&) noexcept = default;

		public:
			bool operator==(const http_response& other) const
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

				this->body().serialize(body_buffer);

				this->content_length(body_buffer.size());

				headline =
					std::format("{} {} {}\r\n", virgo::from_string_version(this->version()),
								static_cast<int>(this->result()), virgo::from_status_string(this->result()).data());

				for (auto& s : this->fields())
				{
					headline += std::format("{}: {}\r\n", s.first, s.second);
				}

				headline += "\r\n";

				buffer.sputn(headline.c_str(), headline.size());

				if (body_buffer.size() != 0)
				{
					buffer.sputn((char*)body_buffer.data().data(), body_buffer.data().size());
				}
			}
		};

		template <virgo::http_method Method, typename Header, typename Body>
		std::ostream& operator<<(std::ostream& os, const http_response<Method, Header, Body>& req)
		{
			req << os;

			return os;
		}
	} // namespace virgo

	template <virgo::http_method Method, typename Header, typename Body>
	struct is_message_type<virgo::http_response<Method, Header, Body>> : std::true_type
	{};
} // namespace aquarius