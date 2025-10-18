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
			, name_()
		{}
	} // namespace lazytool
} // namespace aquarius