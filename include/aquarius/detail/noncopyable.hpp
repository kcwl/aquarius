#pragma once


namespace aquarius
{
	namespace core
	{
		class noncopyable
		{
		protected:
			noncopyable() = default;
			~noncopyable() = default;

			noncopyable(const noncopyable&) = delete;
			noncopyable& operator=(const noncopyable&) = delete;
		};
	}
}