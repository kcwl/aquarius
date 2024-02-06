#pragma once
#include "defines.h"
#include "key_word.hpp"
#include <string>

namespace aquarius
{
	template <typename T, std::size_t... I>
	bool spilt(const char elem, T&& t, std::index_sequence<I...>)
	{
		return ((elem != std::get<I>(std::forward<T>(t))) && ...);
	}

	template <typename... Args>
	bool is_spilt(const char elem, Args&&... args)
	{
		return spilt(elem, std::make_tuple(std::forward<Args>(args)...),
									  std::make_index_sequence<sizeof...(args)>{});
	}

	template <typename... Args>
	std::string trip(std::string& str, Args&&... args)
	{
		for (auto iter = str.begin(); iter != str.end();)
		{
			if (is_spilt(*iter, std::forward<Args>(args)...))
				iter++;
			else
				iter = str.erase(iter);
		}

		return str;
	}

	template<typename _Stream, typename... _Args>
	bool skip_if(_Stream& stream, _Args&&... args)
	{
		while (!stream.eof())
		{
			auto cur = stream.peek();

			if (is_spilt(static_cast<char>(cur), std::forward<_Args>(args)...))
				break;

			stream.get();
		}

		return !stream.fail();
	}

	inline std::string substr_num(const std::string& word)
	{
		int pos = 0;

		for (auto& s : word)
		{
			if (!std::isalnum(s))
				break;

			pos++;
		}

		return word.substr(0, pos);
	}

	inline bool spilt_by(const std::string& keywords, const char sp, std::string& first, std::string& second)
	{
		auto pos = keywords.find_first_of(sp);

		if (pos == std::string::npos)
		{
			return false;
		}

		first = keywords.substr(0, pos);

		second = keywords.substr(pos + 1);

		second = substr_num(second);

		return true;
	}

	inline bool check_key_word(const std::string& key)
	{
		auto iter = key_word.find(key);

		return iter != key_word.end();
	}

	inline bool check_key_type(const std::string& key)
	{
		auto iter = key_type.find(key);

		return iter != key_type.end();
	}

	inline bool check_key_info(const std::string& key)
	{
		return check_key_type(key) || check_key_word(key);
	}

} // namespace elastic