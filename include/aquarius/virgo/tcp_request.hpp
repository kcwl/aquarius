#pragma once
#include <aquarius/ip/concept.hpp>
#include <aquarius/ip/default_selector.hpp>
#include <aquarius/ip/protocol.hpp>
#include <aquarius/ip/tcp/tcp_header.hpp>
#include <aquarius/serialize/binary.hpp>
#include <aquarius/virgo/basic_tcp_protocol.hpp>

namespace aquarius
{
	namespace virgo
	{
		template <detail::string_literal Router, typename Body>
		class tcp_request : public basic_tcp_protocol<true, tcp_request_header, Body>
		{
		public:
			using base = basic_tcp_protocol<true, tcp_request_header, Body>;

			using base::has_request;

			constexpr static auto router = detail::bind_param<Router>::value;

		public:
			tcp_request() = default;

			virtual ~tcp_request() = default;

			tcp_request(const tcp_request& other) = default;

			tcp_request& operator=(const tcp_request& other) = default;

			tcp_request(tcp_request&& other) noexcept = default;

			tcp_request& operator=(tcp_request&& other) noexcept = default;

		public:
			bool operator==(const tcp_request& other) const
			{
				return base::operator==(other);
			}

			std::ostream& operator<<(std::ostream& os) const
			{
				return base::operator<<(os);
			}

		public:
			bool commit(flex_buffer& buffer)
			{
				buffer.commit(sizeof(uint32_t) + sizeof(uint32_t));

				binary_parse{}.to_datas(router, buffer);

				this->header().serialize(buffer);

				this->body().serialize(buffer);

				buffer.pubseekpos(0, std::ios::beg);

				uint32_t length = static_cast<uint32_t>(buffer.size());

				buffer.sputn((char*)&length, sizeof(uint32_t));

				auto seq = detail::uuid_generator()();
				buffer.sputn((char*)&seq, sizeof(uint32_t));

				this->seq_number(seq);

				return true;
			}

			void consume(flex_buffer& buffer)
			{
				this->header().deserialize(buffer);

				this->body().deserialize(buffer);
			}
		};

		template <detail::string_literal Router, typename Body>
		std::ostream& operator<<(std::ostream& os, const tcp_request<Router, Body>& req)
		{
			req << os;

			return os;
		}
	} // namespace virgo
	template <detail::string_literal Router, typename Body>
	struct is_message_type<virgo::tcp_request<Router, Body>> : std::true_type
	{};

	template <detail::string_literal Router, typename Body>
	struct handler_tag_traits<virgo::tcp_request<Router, Body>>
	{
		constexpr static auto tag = proto::tcp;

		using selector = ip::tcp_selector;
	};
} // namespace aquarius