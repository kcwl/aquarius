#pragma once
#include "interface.hpp"
#include "primitive.hpp"
#include "serialize.hpp"

#include <istream>

namespace aquarius
{
	class binary_iarchive : public binary_iprimitive<binary_iarchive, uint8_t, std::char_traits<uint8_t>>,
							public interface_iarchive<binary_iarchive>
	{
		friend class interface_iarchive<binary_iarchive>;

	public:
		template <typename _StreamBuffer>
		explicit binary_iarchive(_StreamBuffer& bs)
			: binary_iprimitive<binary_iarchive, uint8_t, std::char_traits<uint8_t>>(bs)
		{}

	private:
		template <typename _Ty>
		bool load_override(_Ty& t)
		{
			bool result = true;

			try
			{
				binary::template deserialize(*this, t);
			}
			catch (...)
			{
				this->roll_back();

				_Ty error{};

				std::swap(t, error);

				result = false;
			}

			return result;
		}
	};

	class binary_oarchive : public binary_oprimitive<binary_oarchive, uint8_t, std::char_traits<uint8_t>>,
							public interface_oarchive<binary_oarchive>
	{
		friend class interface_oarchive<binary_oarchive>;

	public:
		template <typename _StreamBuffer>
		explicit binary_oarchive(_StreamBuffer& bsb)
			: binary_oprimitive<binary_oarchive, uint8_t, std::char_traits<uint8_t>>(bsb)
		{}

	private:
		template <typename _Ty>
		void save_override(_Ty&& t)
		{
			binary::template serialize(*this, std::forward<_Ty>(t));
		}
	};

	template <typename _Ty, typename _Buffer>
	bool to_binary(_Ty&& t, _Buffer& buffer)
	{
		binary_oarchive oa(buffer);

		oa << std::forward<_Ty>(t);

		return oa.good();
	}

	template <typename _Ty, typename _Buffer>
	bool from_binary(_Ty& t, _Buffer& buffer)
	{
		binary_iarchive ia(buffer);

		ia >> t;

		return ia.good();
	}

} // namespace elastic