#pragma once
#include <type_traits>

namespace aquarius
{
	namespace core
	{
		template<typename T>
		class singleton_wrapper : public T
		{
			
		};

		template<typename T>
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
				static singleton_wrapper<T> t;

				return static_cast<T&>(t);
			}

		protected:
			singleton() {}

		private:
			static T* instance_;
		};
	}
}

template<typename T>
T* aquarius::core::singleton<T>::instance_ = &singleton<T>::instance();
