#pragma once
#include <string>
#include <aquarius/system/error_code/system_errc.hpp>

namespace aquarius
{
	namespace system
	{
		class system_error_message
		{
		public:
			system_error_message(int err_code)
				: message_(make_error_message(err_code))
			{
				
			}

		private:
			std::string make_error_message(int err_code)
			{
				return system_errc_map(err_code);
			}

		public:
			std::string message_;
		};
	}
}