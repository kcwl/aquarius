#pragma once
#include <aquarius/serialize/binary_iarchive.hpp>
#include <aquarius/serialize/binary_iprimitive.hpp>
#include <aquarius/serialize/binary_oarchive.hpp>
#include <aquarius/serialize/binary_oprimitive.hpp>
#include <aquarius/serialize/binary_serialize.hpp>
#include <aquarius/serialize/flex_buffer.hpp>
#include <aquarius/serialize/macro_expand.hpp>
#include <aquarius/serialize/type_define.hpp>

namespace aquarius
{
	template <typename _Ty, typename _Elem, typename _Traits = std::char_traits<_Elem>>
	bool to_binary(_Ty&& t, flex_buffer<_Elem, _Traits>& buffer)
	{
		binary_oarchive oa(buffer);

		oa << std::forward<_Ty>(t);

		return buffer.success();
	}

	template <typename _Ty, typename _Elem, typename _Traits = std::char_traits<_Elem>>
	bool from_binary(_Ty& t, flex_buffer<_Elem, _Traits>& buffer)
	{
		binary_iarchive ia(buffer);

		ia >> t;

		return buffer.success();
	}
} // namespace aquarius

#ifndef AQUARIUS_ACCESS
#define AQUARIUS_ACCESS(...)                                                                                           \
private:                                                                                                               \
	friend class aquarius::access;                                                                                     \
	template <typename _Archive>                                                                                       \
	void serialize(_Archive& ar)                                                                                       \
	{                                                                                                                  \
		AQUARIUS_BIND_WITH(ar&, __VA_ARGS__);                                                                          \
	}
#endif

#ifndef AQUARIUS_ACCESS_IF
#define AQUARIUS_ACCESS_IF(base, ...)                                                                                  \
private:                                                                                                               \
	friend class aquarius::access;                                                                                     \
	template <typename _Archive>                                                                                       \
	void serialize(_Archive& ar)                                                                                       \
	{                                                                                                                  \
		ar& aquarius::base_object<base>(*this);                                                                        \
		AQUARIUS_BIND_WITH(ar&, __VA_ARGS__);                                                                          \
	}
#endif