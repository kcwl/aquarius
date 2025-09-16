#pragma once
#include <aquarius/virgo/basic_tcp_protocol.hpp>

namespace aquarius
{
	namespace virgo
	{

		template <detail::string_literal Router, typename Header, typename Body>
		class tcp_response
			: public basic_tcp_protocol<false, Router, typename Header, typename Body, std::allocator<Body>>
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
			template <typename T>
			bool commit(detail::flex_buffer<T>& buffer)
			{
				if (!header_parse_.to_datas(Router, buffer))
					return false;

				auto pos = buffer.tellg();

				buffer.commit(sizeof(uint32_t));

				body_parse_.to_datas(this->result(), buffer);

				body_parse_.to_datas(this->timestamp(), buffer);

				body_parse_.to_datas(this->version(), buffer);

				body_parse_.to_datas(this->header(), buffer);

				body_parse_.to_datas(this->body(), buffer);

				auto len = buffer.tellg() - pos;
				std::copy((char*)&len, (char*)(&len + 1), buffer.data() + pos);

				return true;
			}

			template <typename T>
			void consume(detail::flex_buffer<T>& buffer)
			{
				this->timestamp(body_parse_.from_datas<int64_t>(buffer));

				this->version(body_parse_.from_datas<int32_t>(buffer));

				this->result() = body_parse_.from_datas<http_status>(buffer);

				this->header() = body_parse_.from_datas<header_t>(buffer);

				this->body() = body_parse_.from_datas<body_t>(buffer);
			}

		private:
			virgo::pod_parse header_parse_;

			virgo::binary_parse body_parse_;
		};
	} // namespace virgo
} // namespace aquarius