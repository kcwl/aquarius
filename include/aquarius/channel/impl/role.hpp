#pragma once
#include <string>

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
				virtual void apply(const std::string& command, std::size_t id) {}

			public:
				std::size_t uuid_;
			};
		} // namespace impl
	}	  // namespace channel
} // namespace aquarius