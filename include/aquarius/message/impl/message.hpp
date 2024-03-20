#pragma once
#include <aquarius/system/visitor.hpp>

namespace aquarius
{
	namespace impl
	{
		class basic_message : public system::visitable
		{
		public:
			basic_message() = default;
			virtual ~basic_message() = default;

		public:
			DEFINE_VISITABLE()
		};
	}
}