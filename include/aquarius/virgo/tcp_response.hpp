#pragma once
#include <aquarius/ip/concept.hpp>
#include <aquarius/serialize/binary.hpp>
#include <aquarius/virgo/basic_tcp_protocol.hpp>

namespace aquarius
{
	namespace virgo
	{
		template <typename Body>
		class tcp_response : public basic_tcp_protocol<false, tcp_response_header, Body>
		{
		public:
			using base = basic_tcp_protocol<false, tcp_response_header, Body>;

			using base::has_request;

		public:
			tcp_response() = default;

			virtual ~tcp_response() = default;

			tcp_response(const tcp_response& other) = default;

			tcp_response& operator=(const tcp_response& other) = default;

			tcp_response(tcp_response&& other) noexcept = default;

			tcp_response& operator=(tcp_response&& other) noexcept = default;

		public:
			bool operator==(const tcp_response& other) const
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

				this->header().serialize(buffer);

				this->body().serialize(buffer);

				auto size = static_cast<uint32_t>(buffer.size());

				buffer.pubseekpos(0, std::ios::in);

				buffer.sputn((char*)&size, sizeof(uint32_t));

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

		template <typename Body>
		std::ostream& operator<<(std::ostream& os, const tcp_response<Body>& req)
		{
			req << os;

			return os;
		}
	} // namespace virgo

	template <typename Body>
	struct is_message_type<virgo::tcp_response<Body>> : std::true_type
	{};
} // namespace aquarius