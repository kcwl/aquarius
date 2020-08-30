#pragma once
#include <type_traits>

namespace aquarius
{
	namespace detail
	{
		template<class T>
		class singleton
		{
		public:
			static T& instance()
			{
				return get_instance();
			}
		private:
			static T& get_instance()
			{
				class singleton_wrapper : public T {};

				static singleton_wrapper* t = nullptr;

				!t ? t = new singleton_wrapper() : 0;

				return static_cast<T&>(*std::add_const_t<singleton_wrapper*>(t));
			}

		private:
			static T& instance_;
		};

		template<class T>
		T& singleton<T>::instance_ = singleton<T>::instance();
	}
}