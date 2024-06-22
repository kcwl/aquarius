#pragma once

namespace aquarius
{
	template <typename _Topic, typename _Func>
	class visitor
	{
	public:
		virtual bool accept(const _Topic&, _Func&&) = 0;
	};
} // namespace aquarius