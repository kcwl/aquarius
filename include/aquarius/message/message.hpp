#pragma once
#include <cstddef>
#include "body.hpp"
#include "field.hpp"
#include "header.hpp"

namespace aquarius
{
	namespace message
	{
		template<bool Request, typename _Header, typename _Body, uint32_t N>
		class basic_message : protected header<Request, fields<_Header>>
		{
		public:
			using header_type =_Header;
			using body_type = _Body;


			enum { Number = N };

		public:
			header_type& header() noexcept
			{
				return this->get();
			}

			const header_type& header() const noexcept
			{
				return this->get();
			}

			body_type& body() noexcept
			{
				return body_.get();
			}

			const body_type& body() const noexcept
			{
				return body_.get();
			}

			template<typename _Stream>
			bool parse_message(_Stream& stream)
			{
				stream.template start<io_state::read>();

				if (!this->parse_bytes(stream))
				{
					stream.template transback<io_state::read>();

					return false;
				}

				if (!body_.parse_bytes(stream))
				{
					stream.template transback<io_state::read>();

					return false;
				}

				return true;
			}

			template<typename _Stream>
			bool to_message(_Stream& stream)
			{
				stream.template start<io_state::write>();

				if (!this->to_bytes(stream))
				{
					stream.template transback<io_state::write>();

					return false;
				}
				
				if (!body_.to_bytes(stream))
				{
					stream.template transback<io_state::write>();

					return false;
				}

				return true;
			}

		private:
			body_of<fields<body_type>> body_;
		};

	}
}