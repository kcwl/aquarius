#pragma once
#include <type_traits>

namespace aquarius
{
	namespace detail
	{
		template <typename _Ty>
		class singleton_wrapper : public _Ty
		{};

		template <typename _Ty>
		class singleton
		{
		public:
			static _Ty& instance() { return get_instance(); }

		private:
			static _Ty& get_instance()
			{
				static singleton_wrapper<_Ty> t;

				return static_cast<_Ty&>(t);
			}

		protected:
			singleton() {}

		private:
			static _Ty* instance_;
		};
	} // namespace detail
} // namespace aquarius

template <typename _Ty>
_Ty* aquarius::detail::singleton<_Ty>::instance_ = &singleton<_Ty>::instance();
