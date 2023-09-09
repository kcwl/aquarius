#pragma once
#include <openssl/md5.h>
#include <string>

namespace aquarius
{
	template <typename _Ty>
	std::string md5_hash(const std::basic_string<_Ty, std::char_traits<_Ty>, std::allocator<_Ty>>& src)
	{
		std::string result{};
		result.resize(MD5_DIGEST_LENGTH * 2);

		MD5((uint8_t*)src.data(), src.size(), (uint8_t*)result.data());

		return result;
	}
} // namespace aquarius