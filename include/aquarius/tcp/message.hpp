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
			enum { NUMBER = Number };

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

			bool serialize(ftstream& buf)
			{
				std::size_t body_size = body_.ByteSizeLong();
				if (buf.size() < body_size)
					return false;

				body_.SerializeToArray(buf.data(), static_cast<int>(body_size));

				return true;
			}

			void deserialize(ftstream& buf)
			{
				buf << header().get();

				body_.ParseFromArray(buf.data(), static_cast<int>(buf.size()));
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