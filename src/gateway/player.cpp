#include "player.h"
#include <aquarius/logger.hpp>

namespace aquarius
{
	namespace gateway
	{
		void player_define::inits(const init&)
		{
			XLOG_DEBUG() << "handling init" << std::endl;
		}

		void player_define::verify_user_handler(const verify&)
		{
			XLOG_DEBUG() << "handling verify" << std::endl;
		}

		void player_define::verify_complete_handler(const complete&)
		{
			XLOG_DEBUG() << "handling verify_complete" << std::endl;
		}

		player::player(std::size_t id)
			: id_(id)
		{}
	} // namespace gateway
} // namespace aquarius