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

		void player_define::verify_user_handler(const verify_user&)
		{
			XLOG_DEBUG() << "handling verify_user" << std::endl;
		}

		void player_define::verify_passwd_handler(const verify_passwd&)
		{
			XLOG_DEBUG() << "handling verify_passwd" << std::endl;
		}

		void player_define::verify_complete_handler(const verify_complete&)
		{
			XLOG_DEBUG() << "handling verify_complete" << std::endl;
		}
	} // namespace gateway
} // namespace aquarius