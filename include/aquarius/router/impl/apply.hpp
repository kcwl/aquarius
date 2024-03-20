#pragma once
#include <memory>
#include <aquarius/system/type_traits.hpp>

namespace aquarius
{
	namespace impl
	{
		class basic_message;
	}
}

namespace aquarius
{
	template <typename _Func>
	struct invoker
	{
		static bool apply(const _Func& f, std::shared_ptr<impl::basic_message> msg)
		{
			using type = system::function_traits<_Func>::type;

			using element_t = typename type::element_type;

			auto ptr = std::dynamic_pointer_cast<element_t>(msg);

			if (!ptr)
				return false;

			f(ptr);

			return true;
		}
	};
}