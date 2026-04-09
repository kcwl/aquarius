#pragma once
#include <aquarius/resource/config_register.hpp>
#include <aquarius/resource/global_resource.hpp>

#define AQUARIUS_MSYQL_CONFIG()                                                                                        \
	class msyql_config_regist;                                                                                         \
	static aquarius::config_register<msyql_config_regist> __msyql_regist();                                              \
	class msyql_config_regist                                                                                          \
	{                                                                                                                  \
	public:                                                                                                            \
		msyql_config_regist();                                                                                         \
                                                                                                                       \
	private:                                                                                                           \
		aquarius::global_resource::mysql_config& mysql;                                                                \
	};                                                                                                                 \
	msyql_config_regist::msyql_config_regist()                                                                         \
		: mysql(aquarius::global_resource::get_mutable_instance().mysql())
