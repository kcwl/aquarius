#pragma once
#include <aquarius/tbl/engine/field.hpp>
#include <bitset>

namespace aquarius
{
	namespace tbl
	{
		template <std::size_t I, typename... Args>
		class bit : public field<Args...>, public std::bitset<I>
		{
			static_assert(I < 65, "bit index must be less than 65");

		public:
			constexpr static auto get_type_name()
			{
				constexpr static auto type_name = " bit("sv;
				constexpr static auto end = ")"sv;

				constexpr static auto num_str = int_to_string<I>();

				return concat_v<type_name, num_str, end>;
			}

			void set_value(std::stringstream& ss)
			{
				(void)ss;
				return;
			}

		public:
			operator std::bitset<I>() const
			{
				return *this;
			}
		};
	} // namespace tbl
} // namespace aquarius