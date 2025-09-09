#pragma once
#include <aquarius/virgo/basic_protocol_message.hpp>
#include <aquarius/detail/flex_buffer.hpp>
#include <aquarius/detail/binary.hpp>
#include <aquarius/virgo/tcp_header.hpp>
#include <aquarius/error_code.hpp>

namespace aquarius
{
	namespace virgo
	{
		template <bool IsRequest, typename Header, typename Body>
		class basic_tcp_message
			: public basic_protocol_message<IsRequest, Header, tcp_header, Body, detail::binary_parse>
		{
			using base_type = basic_protocol_message<IsRequest, Header, tcp_header, Body, detail::binary_parse>;

		public:
			using header_t = Header;

			using body_ptr_t = std::add_pointer_t<Body>;

		public:
			basic_tcp_message() = default;

		public:
			template <typename T>
			std::expected<bool, error_code> commit(detail::flex_buffer<T>& buffer)
			{
				auto result = base_type::commit(buffer);

				if (!result.has_value())
				{
					return result;
				}

				result = this->header().commit(buffer);

				if (!result.has_value())
				{
					return result;
				}

				this->parse_.to_datas(this->body(), buffer);

				return true;
			}

			template <typename T>
			std::expected<bool, error_code> consume(detail::flex_buffer<T>& buffer)
			{
				auto result = this->header().consume(buffer);

				if (!result.has_value())
				{
					return result;
				}

				this->body() = this->parse_.template from_datas<Body>(buffer);

				return true;
			}

		protected:
			header_t header_;
		};
	} // namespace virgo
} // namespace aquarius