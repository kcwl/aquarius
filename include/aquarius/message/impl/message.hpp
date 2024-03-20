#pragma once
#include <aquarius/message/impl/visitable.hpp>

namespace aquarius
{
	namespace impl
	{
		class basic_message : public visitable
		{
		public:
			basic_message() = default;
			virtual ~basic_message() = default;

		public:
			DEFINE_VISITABLE()
		};
	}
}