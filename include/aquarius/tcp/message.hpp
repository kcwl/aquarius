#pragma once
#include "field.hpp"
#include "header.hpp"
#include "header_value.hpp"
#include "../stream.hpp"

#pragma warning(disable:4100)

namespace aquarius
{
	namespace tcp
	{
		template<bool Request, typename Body, std::size_t Number>
		class message 
			: public header<Request,header_fields>
		{
		public:
			using header_type = header<Request, header_fields>;

		public:
			message()
				: body_()
			{

			}

			message(header_type header)
				: header_type(header)
				, body_()
			{

			}

			message(message&&) = default;

			message(message const&) = default;

			message& operator=(message&&) = default;

			message& operator=(message const&) = default;

		public:
			header_type& header()
			{
				return *this;
			}

			header_type& header() const
			{
				return *this;
			}

			Body& body()
			{
				return body_;
			}

			Body& body() const
			{
				return body_;
			}

			std::string key()
			{
				return std::to_string(Number);
			}

			int parse_bytes(ftstream& buf)
			{
				if (buf.size() != sizeof(Body))
				{
					buf.consume(static_cast<int>(0 - sizeof(header_fields::value_t)));

					return 0;
				}

				body_.serialize(buf);

				return 1;
			}

			int to_bytes(ftstream& buf)
			{
				buf << body_.deserialize();

				return 1;
			}

			void copy(const typename header_type::value_t& hv)
			{
				header_type::copy(hv);
			}

		private:
			Body body_;
		};
	}
}