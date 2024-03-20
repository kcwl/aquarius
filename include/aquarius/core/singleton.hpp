#pragma once

namespace aquarius
{
	template <typename _Ty>
	class singleton
	{
		template <typename _Single>
		struct singleton_wrapper : _Single
		{};

	public:
		static _Ty& instance()
		{
			return get_instance();
		}

	private:
		static _Ty& get_instance()
		{
			static singleton_wrapper<_Ty> t;

			return static_cast<_Ty&>(t);
		}

	protected:
		singleton()
		{}

	private:
		static _Ty* instance_;
	};
} // namespace aquarius

template <typename _Ty>
_Ty* aquarius::singleton<_Ty>::instance_ = &singleton<_Ty>::instance();
