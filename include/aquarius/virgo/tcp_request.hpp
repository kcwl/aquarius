#pragma once
#include <aquarius/virgo/basic_tcp_protocol.hpp>
#include <aquarius/serialize/binary.hpp>

namespace aquarius
{
	namespace virgo
	{
		template <detail::string_literal Router, typename Header, typename Body>
		class tcp_request
			: public basic_tcp_protocol<true, Router, typename Header, typename Body, std::allocator<Body>>
		{
		public:
			using base = basic_tcp_protocol<true, Router, Header, Body, std::allocator<Body>>;

			using base::router;

			using typename base::header_t;

			using typename base::body_t;

			using base::has_request;

		public:
			tcp_request() = default;

		public:
			template <typename T>
			bool commit(detail::flex_buffer<T>& buffer)
			{
				if (!body_parse_.to_datas(Router, buffer))
					return false;

				auto pos = buffer.tellg();

				buffer.commit(sizeof(uint32_t));

				if (!body_parse_.to_datas(this->timestamp(), buffer))
					return false;

				if (!body_parse_.to_datas(this->version(), buffer))
					return false;

				if (!body_parse_.to_datas(this->header(), buffer))
					return false;

				if (!body_parse_.to_datas(this->body(), buffer))
					return false;

				auto len = buffer.tellg() - pos;
				std::copy((char*)&len, (char*)(&len + 1),
						  buffer.data() + pos);

				return true;
			}

			template<typename T>
			void consume(detail::flex_buffer<T>& buffer)
			{
				this->timestamp(body_parse_.from_datas<int64_t>(buffer));

				this->version(body_parse_.from_datas<int32_t>(buffer));

				this->header() = body_parse_.from_datas<header_t>(buffer);

				this->body() = body_parse_.from_datas<body_t>(buffer);
			}

		private:
			virgo::binary_parse body_parse_;
		};
	} // namespace virgo
} // namespace aquarius