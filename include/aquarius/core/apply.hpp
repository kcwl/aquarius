#pragma once
#include <aquarius/core/concepts.hpp>
#include <memory>

namespace aquarius
{
	template <typename _Func, typename _Message>
	struct invoker
	{
		static bool apply(const _Func& f, std::shared_ptr<_Message> msg)
		{
			using type = function_traits<_Func>::type;

			using element_t = typename type::element_type;

			auto ptr = std::dynamic_pointer_cast<element_t>(msg);

			if (!ptr)
				return false;

			f(ptr);

			return true;
		}
	};
} // namespace aquarius