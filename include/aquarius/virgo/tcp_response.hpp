#pragma once
#include <aquarius/virgo/basic_tcp_protocol.hpp>
#include <aquarius/serialize/binary.hpp>
#include <aquarius/ip/concept.hpp>

namespace aquarius
{
	namespace virgo
	{

		template <typename Header, typename Body>
		class tcp_response : public basic_tcp_protocol<false, Header, Body>
		{
		public:
			using base = basic_tcp_protocol<false, Header, Body>;

			using base::has_request;

		public:
			tcp_response()
				: base()
				, parse_()
			{}

			virtual ~tcp_response() = default;

			tcp_response(const tcp_response& other)
				: base(other)
				, parse_(other.parse_)
			{}

			tcp_response& operator=(const tcp_response& other)
			{
				if (this != std::addressof(other))
				{
					base::operator=(other);
					parse_ = other.parse_;
				}
				return *this;
			}

			tcp_response(tcp_response&& other) noexcept
				: base(std::move(other))
				, parse_(std::exchange(other.parse_, {}))
			{}

			tcp_response& operator=(tcp_response&& other) noexcept
			{
				if (this != std::addressof(other))
				{
					base::operator=(std::move(other));
					parse_ = std::exchange(other.parse_, {});
				}
				return *this;
			}

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
				flex_buffer buf{};

				parse_.to_datas(this->result(), buf);

				parse_.to_datas(this->timestamp(), buf);

				parse_.to_datas(this->version(), buf);

				this->header().serialize(buf);

				this->body().serialize(buf);

				auto size = static_cast<uint32_t>(buf.size());

				buffer.sputn((char*)&size, sizeof(uint32_t));

				buffer.sputn((char*)buf.data().data(), buf.data().size());

				return true;
			}

			void consume(flex_buffer& buffer)
			{
				this->result(parse_.from_datas<int32_t>(buffer));

				this->timestamp(parse_.from_datas<int64_t>(buffer));

				this->version(parse_.from_datas<int32_t>(buffer));

				this->header().deserialize(buffer);

				this->body().deserialize(buffer);
			}

		private:
			binary_parse parse_;
		};

		template <typename Header, typename Body>
		std::ostream& operator<<(std::ostream& os, const tcp_response<Header, Body>& req)
		{
			req << os;

			return os;
		}
	} // namespace virgo

	template <typename Header, typename Body>
	struct is_message_type<virgo::tcp_response<Header, Body>> : std::true_type
	{};
} // namespace aquarius