#pragma once

namespace aquarius
{
	namespace config
	{
		template<typename T>
		struct value_from {};
	}
}

template<typename T>
void config_tag_invoke(const aquarius::config::value_from<T>&, T& a)
{
	return;
}

#define CONFIG_MICRO(kind, a) \
void config_tag_invoke(const aquarius::config::value_from<kind>&, kind& a) \