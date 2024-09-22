#pragma once
#include <aquarius/serialize/binary_oprimitive.hpp>
#include <aquarius/serialize/binary_serialize.hpp>
#include <aquarius/serialize/detail/interface_oarchive.hpp>

namespace aquarius
{
	class binary_oarchive : public detail::interface_oarchive<binary_oarchive>,
							public binary_oprimitive<binary_oarchive, uint8_t>
	{
		friend class detail::interface_oarchive<binary_oarchive>;

	public:
		template <typename _StreamBuffer>
		explicit binary_oarchive(_StreamBuffer& bsb)
			: binary_oprimitive<binary_oarchive, uint8_t, std::char_traits<uint8_t>>(bsb)
		{}

		virtual ~binary_oarchive() = default;

	private:
		template<typename _Ty>
		void save_override(_Ty&& t)
		{
			binary::serialize(*this, std::forward<_Ty>(t));
		}
	};
} // namespace aquarius