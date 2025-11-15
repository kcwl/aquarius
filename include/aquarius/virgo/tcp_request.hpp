#pragma once
#include <aquarius/virgo/basic_tcp_protocol.hpp>
#include <aquarius/serialize/binary.hpp>
#include <aquarius/ip/concept.hpp>

namespace aquarius
{
	namespace virgo
	{
		template <detail::string_literal Router, typename Header, typename Body>
		class tcp_request : public basic_tcp_protocol<true, Header, Body>
		{
		public:
			using base = basic_tcp_protocol<true, Header, Body>;

			using base::has_request;

			constexpr static auto router = detail::bind_param<Router>::value;

		public:
			tcp_request()
				: base()
				, parse_()
			{}

			virtual ~tcp_request() = default;

			tcp_request(const tcp_request& other)
				: base(other)
				, parse_(other.parse_)
			{}
			tcp_request& operator=(const tcp_request& other)
			{
				if (this != std::addressof(other))
				{
					base::operator=(other);
					parse_ = other.parse_;
				}
				return *this;
			}

			tcp_request(tcp_request&& other) noexcept
				: base(std::move(other))
				, parse_(std::exchange(other.parse_, {}))
			{}

			tcp_request& operator=(tcp_request&& other) noexcept
			{
				if (this != std::addressof(other))
				{
					base::operator=(std::move(other));
					parse_ = std::exchange(other.parse_, {});
				}
				return *this;
			}

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
				parse_.to_datas(this->timestamp(), buffer);

				parse_.to_datas(this->version(), buffer);

				this->header().serialize(buffer);

				this->body().serialize(buffer);

				return true;
			}

			void consume(flex_buffer& buffer)
			{
				this->timestamp(parse_.from_datas<int64_t>(buffer));

				this->version(parse_.from_datas<int32_t>(buffer));

				this->header().deserialize(buffer);

				this->body().deserialize(buffer);
			}

		private:
			binary_parse parse_;
		};

		template <detail::string_literal Router, typename Header, typename Body>
		std::ostream& operator<<(std::ostream& os, const tcp_request<Router, Header, Body>& req)
		{
			req << os;

			return os;
		}
	} // namespace virgo
	template <detail::string_literal Router, typename Header, typename Body>
	struct is_message_type<virgo::tcp_request<Router, Header, Body>> : std::true_type
	{};
} // namespace aquarius