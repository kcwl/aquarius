#pragma once

namespace aquarius
{
	namespace tbl
	{
		template <typename T>
		struct _bool
		{
			constexpr static auto name = " bool"sv;

			static constexpr auto max_length = 1;
		};
	} // namespace tbl
} // namespace aquarius