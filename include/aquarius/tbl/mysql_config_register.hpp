#pragma once
#include <aquarius/resource/config_register.hpp>

namespace aquarius
{
#define AQUARIUS_MSYQL_CONFIG()                                                                                        \
	class msyql_config_regist;                                                                                         \
	static config_register<msyql_config_regist> __msyql_regist();                                                      \
	class msyql_config_regist                                                                                          \
	{                                                                                                                  \
	public:                                                                                                            \
		msyql_config_regist();                                                                                         \
                                                                                                                       \
	private:                                                                                                           \
		global_resource::mysql_config& mysql;                                                                          \
	};                                                                                                                 \
	msyql_config_regist::msyql_config_regist()                                                                         \
		: mysql(global_resource::get_mutable_instance().mysql())

} // namespace aquarius