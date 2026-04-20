#pragma once

namespace aquarius
{
	template<class T>
	struct cfg_from_tag {};

	template<typename T>
	void value_from_impl(cfg_from_tag<T> tag, T& v)
	{
		return cfg_invoke(tag, v);
	}

	template<typename T>
	void cfg_value_from(T& v)
	{
		return value_from_impl(cfg_from_tag<T>(), v);
	}
}

#define CONFIG_MICRO(kind, a) \
inline void cfg_invoke(aquarius::cfg_from_tag<kind>, kind& a)