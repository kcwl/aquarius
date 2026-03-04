#include <aquarius/tbl.hpp>

struct account
{
	aquarius::tbl::string account;
	aquarius::tbl::string passwd;
	constexpr static auto member()
	{
		return std::make_tuple(
			&account::account,
			&account::passwd
		);
	}
	constexpr static auto member_name()
	{
		return std::array{
			"account"sv,
			"passwd"sv
		};
	}
};