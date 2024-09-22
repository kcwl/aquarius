#pragma once
#include <string>
#include <aquarius/serialize/detail/basic_primitive.hpp>

namespace aquarius
{
	template <typename _Archive, typename _Elem, typename _Traits = std::char_traits<_Elem>>
	class binary_oprimitive : detail::basic_primitive<_Elem, _Traits>
	{
		using base_type = detail::basic_primitive<_Elem, _Traits>;

	public:
		using value_type = typename base_type::value_type;

		using traits_t = typename base_type::traits_t;

	protected:
		binary_oprimitive(flex_buffer<value_type, traits_t>& bs)
			: detail::basic_primitive<value_type, traits_t>(bs)
		{}

		virtual ~binary_oprimitive() = default;

	public:
		bool save(const value_type* data, const std::size_t size)
		{
			return this->streambuf_.save(data, size);
		}
	};
} // namespace elastic