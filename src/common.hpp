#pragma once
#include "defines.h"

#include <string>

namespace elastic
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

		return stream.good();
	}

	inline bool spilt_by(const std::string& keywords, const char sp, std::string& first, std::string& second)
	{
		auto pos = keywords.find_first_of(sp);

		if (pos == std::string::npos)
		{
			first = keywords;
		}

		first = keywords.substr(0, pos);

		second = keywords.substr(pos + 1);

		pos = second.find_first_of('/');

		if (pos != std::string::npos)
		{
			second = second.substr(0, pos);
		}

		return true;
	}

} // namespace elastic