#pragma once
#include <aquarius/basic_protocol.hpp>

namespace aquarius
{
	namespace virgo
	{

		template <bool Request, typename Header, typename Body, typename Allocator = std::allocator<Body>>
		class basic_tcp_protocol : public basic_protocol<Header, std::add_pointer_t<Body>, Allocator>
		{
		public:
			using base = basic_protocol<Header, std::add_pointer_t<Body>, Allocator>;

			using typename base::header_t;

			using typename base::body_t;

			using typename base::seq_t;

			using typename base::version_t;

			constexpr static auto has_request = Request;

			using length_offset_t = uint16_t;

		public:
			basic_tcp_protocol() = default;

			virtual ~basic_tcp_protocol() = default;

			basic_tcp_protocol(const basic_tcp_protocol&) = default;

			basic_tcp_protocol& operator=(const basic_tcp_protocol& other) = default;

			basic_tcp_protocol(basic_tcp_protocol&& other) noexcept = default;

			basic_tcp_protocol& operator=(basic_tcp_protocol&& other) noexcept = default;

		public:
			virtual bool commit(flex_buffer& buffer) override
			{
				constexpr auto length_offset = sizeof(length_offset_t);

				buffer.commit(length_offset);

				binary_parse parse{};

				parse.to_datas(router(), buffer);

				parse.to_datas(this->seq_number(), buffer);

				parse.to_datas(this->version(), buffer);

				this->header().serialize(buffer);

				this->body().serialize(buffer);

				buffer.pubseekpos(0, std::ios::in);

				length_offset_t length = static_cast<length_offset_t>(buffer.size() - length_offset);

				buffer.sputn((char*)&length, length_offset);

				return true;
			}

			virtual bool consume_header(flex_buffer& buffer) override
			{
				binary_parse parse{};

				this->seq_number() = parse.from_datas<seq_t>(buffer);

				this->version() = parse.from_datas<version_t>(buffer);

				this->header().deserialize(buffer);

				return true;
			}

		protected:
			virtual std::string router()
			{
				return {};
			}
		};

	} // namespace virgo
} // namespace aquarius