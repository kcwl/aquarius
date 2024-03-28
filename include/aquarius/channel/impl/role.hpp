#pragma once
#include <cstring>

namespace aquarius
{
	namespace channel
	{
		namespace impl
		{
			class role
			{
			public:
				role() = default;
				virtual ~role() = default;

			public:
				std::size_t uuid_;
			};
		} // namespace impl
	}	  // namespace channel
} // namespace aquarius