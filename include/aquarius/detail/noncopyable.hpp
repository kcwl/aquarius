#pragma once

namespace aquarius
{
	namespace detail
	{
		class noncopyable
		{
		protected:
			noncopyable() = default;
			~noncopyable() = default;

			noncopyable(const noncopyable&) = delete;
			noncopyable& operator=(const noncopyable&) = delete;
		};
	} // namespace core
} // namespace aquarius