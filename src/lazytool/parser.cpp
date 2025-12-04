#include "parser.h"

namespace aquarius
{
	namespace lazytool
	{
		parser::parser()
			: parser(struct_type::none)
		{
		}

		parser::parser(struct_type t)
			: struct_type_(t)
		{}

		struct_type parser::structtype()const
		{
			return struct_type_;
		}
	} // namespace lazytool
} // namespace aquarius