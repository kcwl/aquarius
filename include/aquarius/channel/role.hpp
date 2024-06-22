#pragma once
#include <aquarius/channel/visitor.hpp>
#include <memory>

namespace aquarius
{
	template<typename _Topic, typename _Func>
	class channel_role : public visitor<_Topic, _Func>
	{
	public:
		channel_role() = default;
		virtual ~channel_role() = default;

	public:
		//void apply(_Func&& f)
		//{
		//	this->accept(std::forward<_Func>(f));
		//}

		std::size_t uuid() const
		{
			return uuid_;
		}

	public:
		std::size_t uuid_;
	};
} // namespace aquarius