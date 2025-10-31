#pragma once
#include <string>
#include <sstream>
#include <iomanip>

namespace aquarius
{

	inline std::string url_encode(const std::string& str)
	{
		std::stringstream ss{};
		std::string result{};

		for (const auto& s : str)
		{
			if (std::isalnum(s) || s == '-' || s == '_' || s == '.' || s == '~' || s == '&' || s == '?' || s == '=')
			{
				ss << s;
			}
			else
			{
				ss << std::hex << std::setw(2) << std::uppercase << (int)s;
				result.push_back('%');

				result.append(ss.str().substr(ss.str().size() - 2));
				ss.clear();
			}
		}
		return result;
	}

	inline std::string url_decode(const std::string& str)
	{
		std::stringstream ss{};
		std::string result{};

		auto size = str.size();

		for (std::size_t i = 0; i < size;)
		{
			if (str[i] == '%')
			{
				ss << std::hex << str.substr(i + 1, 2);

				int tmp{};
				ss >> std::hex >> tmp;

				ss.clear();

				i += 3;

				result.push_back(static_cast<char>(tmp));
			}
			else
			{
				result.push_back(str[i]);
				i++;
			}
		}

		return result;
	}

	inline std::u8string url_u8_decode(const std::string& str)
	{
		auto decode_seq = url_decode(str);

		std::u8string result{};

		for (const auto& s : decode_seq)
		{
			result.push_back(s);
		}

		return result;
	}

	inline std::string url_u8_encode(const std::u8string& str)
	{
		std::stringstream ss{};

		for (const auto& s : str)
		{
			ss << (char)(uint8_t)(char)s;
		}

		return url_encode(ss.str());
	}

} // namespace aquarius