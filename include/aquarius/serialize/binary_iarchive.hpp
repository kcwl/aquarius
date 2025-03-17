#pragma once
#include <aquarius/serialize/binary_iprimitive.hpp>
#include <aquarius/serialize/binary_serialize.hpp>
#include <aquarius/serialize/detail/interface_iarchive.hpp>

namespace aquarius
{
	class binary_iarchive : public detail::interface_iarchive<binary_iarchive>,
							public binary_iprimitive<binary_iarchive, uint8_t>
	{
		friend class detail::interface_iarchive<binary_iarchive>;

	public:
		template <typename _StreamBuffer>
		explicit binary_iarchive(_StreamBuffer& bs)
			: binary_iprimitive<binary_iarchive, uint8_t, std::char_traits<uint8_t>>(bs)
		{}

		virtual ~binary_iarchive() = default;

	private:
		template <typename _Ty>
		void load_override(_Ty& t)
		{
			primitive_guard lk(this);

			if (!binary::deserialize(*this, t))
			{
				this->failed();
			}
		}
	};
} // namespace aquarius