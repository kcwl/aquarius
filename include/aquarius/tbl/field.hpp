#pragma once
#include <tuple>

namespace aquarius
{
	namespace tbl
	{
        struct primary {};
        struct foreign {};
        struct unique {};
        

        template<typename T>
        constexpr bool unique_index_adaptor()
        {
            bool result = true;

            constexpr auto size = std::tuple_size<T>::value;

            auto f = [&]<std::size_t... I>(std::index_sequence<I...>)
            {
                if ((((std::is_same<std::tuple_element_t<I, T>, key<unique>>::value) || std::is_same_v<std::tuple_element_t<I, T>, index<unique>>) || ...))
                {
                    result = !result;
                }
            };

            f(std::make_index_sequence<size>{});

            return result;
        }

        template<typename T>
        constexpr bool text_index_adaptor()
        {
            bool result = true;

            constexpr auto size = std::tuple_size<T>::value;

            auto f = [&]<std::size_t... I>(std::index_sequence<I...>)
            {
                if ((((std::is_same<std::tuple_element_t<I, T>, key<text>>::value) || std::is_same_v<std::tuple_element_t<I, T>, index<text>>) || ...))
                {
                    result = !result;
                }
            };

            f(std::make_index_sequence<size>{});

            return result;
        }

		template<typename T, typename... Args>
		struct field
		{
			using value_type = T;

			using attr_type = std::tuple<Args...>;

			static_assert(unique_index_adaptor<attr_type>(), "unique index must be use on unique key");

			static_assert(text_index_adaptor<attr_type>(), "text index must be use on text key");
		};
	}
}