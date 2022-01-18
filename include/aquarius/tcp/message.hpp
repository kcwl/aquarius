#pragma once
#include "filed.hpp"
#include "header.hpp"
#include "header_value.hpp"
#include "../detail/stream.hpp"

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
			message() = default;

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

			int parse_bytes(streambuf& buf)
			{
				return 1;
			}

			int to_bytes(streambuf& buf)
			{
				return 1;
			}

		private:
			Body body_;
		};
	}
}