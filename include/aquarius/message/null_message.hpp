#pragma once
#include "../visitor.hpp"

namespace aquarius
{
	class context;

	namespace msg
	{
		struct null_body {};

		template<bool Request, typename Body, std::size_t N>
		class null_baisc_message : public visiable<>
		{
		public:
			null_baisc_message() = default;
			
			~null_baisc_message() = default;

		public:
			null_baisc_message(null_baisc_message const&) = default;

			null_baisc_message(null_baisc_message&&) = default;

			null_baisc_message& operator=(null_baisc_message const&) = default;

			virtual int accept(std::shared_ptr<context> ctx_ptr)
			{
				//return accept_impl<null_baisc_message>(this, ctx_ptr);
				return 0;
			}

			virtual bool parse(ftstream& ar)
			{
				return false;
			}
		};
	}

	using null_message = msg::null_baisc_message<true, msg::null_body, 0>;
}