#pragma once
#include "../visitor.hpp"

namespace aquarius
{
	class context;

	namespace msg
	{
		class ftstream;

		class basic_message : public visiable<>
		{
		public:
			using header_type = int;
		public:
			basic_message() = default;

			~basic_message() = default;


		public:
			basic_message(basic_message const&) = default;

			basic_message(basic_message&&) = default;

			basic_message& operator=(const basic_message&) = default;

			virtual int accept(std::shared_ptr<context> ctx_ptr) { return 0; }

			virtual bool parse_bytes(aquarius::ftstream&) { return false; }

			virtual bool to_bytes(aquarius::ftstream&) { return false; }
		};
	}

	using null_message = msg::basic_message;
}