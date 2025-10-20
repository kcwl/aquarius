#pragma once
#include <aquarius/virgo/basic_tcp_protocol.hpp>
#include <aquarius/serialize/binary.hpp>
#include <aquarius/ip/concept.hpp>


namespace aquarius
{
	namespace virgo
	{

		template <detail::string_literal Router, typename Header, typename Body>
		class tcp_response
			: public basic_tcp_protocol<false, Router, Header, Body, std::allocator<Body>>
		{
		public:
			using base = basic_tcp_protocol<false, Router, Header, Body, std::allocator<Body>>;

			using base::router;

			using base::has_request;

			using typename base::header_t;

			using typename base::body_t;

		public:
			tcp_response() = default;

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
			template <typename T>
			bool commit(flex_buffer<T>& buffer)
			{
				body_parse_.to_datas(this->result(), buffer);

				body_parse_.to_datas(this->timestamp(), buffer);

				body_parse_.to_datas(this->version(), buffer);

				this->header().serialize(buffer);

				this->body().serialize(buffer);

				return true;
			}

			template <typename T>
			void consume(flex_buffer<T>& buffer)
			{
				this->result(body_parse_.from_datas<int32_t>(buffer));

				this->timestamp(body_parse_.from_datas<int64_t>(buffer));

				this->version(body_parse_.from_datas<int32_t>(buffer));

				this->header().deserialize(buffer);

				this->body().deserialize(buffer);
			}

		private:
			binary_parse body_parse_;
		};

		template <detail::string_literal Router, typename Header, typename Body>
		std::ostream& operator<<(std::ostream& os, const tcp_response<Router, Header, Body>& req)
		{
			req << os;

			return os;
		}
	} // namespace virgo

	template<detail::string_literal Router, typename Header, typename Body>
	struct is_message_type<virgo::tcp_response<Router, Header, Body>> : std::true_type {};
} // namespace aquarius