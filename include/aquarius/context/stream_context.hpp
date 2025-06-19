#pragma once
#include <aquarius/detail/context_base.hpp>

namespace aquarius
{
	namespace context
	{
		class stream_context : public context_base
		{
		public:
			stream_context()
				: context_base("stream context", 10s)
			{}
		};

	} // namespace context
} // namespace aquarius