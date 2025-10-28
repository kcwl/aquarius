#pragma once
#include <string>
#include <sstream>
#include <iomanip>

namespace aquarius
{

	inline std::string url_encode(const std::string& str)
	{
		std::stringstream ss{};

		for (const auto& s : str)
		{
			if (std::isalnum(s) || s == '-' || s == '_' || s == '.' || s == '~' || s == '&' || s == '?' || s == '=')
			{
				ss << s;
			}
			else
			{
				ss << '%' << std::hex << std::setw(2) << std::uppercase << static_cast<int>(s);
			}
		}
		return ss.str();
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
				ss << std::hex<<str.substr(i + 1, 2);

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

		

		for (auto iter = decode_seq.begin(); iter != decode_seq.end();)
		{
			int s = *iter;

			if ((s & 0x80) != 0)
			{
				int s1 = *(++iter);

				char8_t temp{};

				temp = (0xe0 | (s & 0xf0)) | ((0x80 | ((s & 0x0f) << 2) | (s1 >> 6)) | (s1 & 0x3f));

				result.push_back(temp);

				iter++;
			}
			else
			{
				result.push_back(s);
				++iter;
			}
		}

		return result;
	}

	inline std::string url_u8_encode(const std::u8string& str)
	{
		std::stringstream ss{};

		char front{};
		char back{};

		for (auto iter = str.begin(); iter != str.end();)
		{
			int s = *iter;

			if ((s & 0x80) != 0)
			{
				int s1 = *(++iter);
				int s2 = *(++iter);
				front = char((s << 4) | ((s1 & 0x3f) >> 2));

				back = char((s1 & 0x3f) << 6) | char(s2 & 0x3f);

				ss << front << back;
				iter++;
			}
			else
			{
				ss << (char)(*iter);
				iter++;
			}
		}

		return url_encode(ss.str());
	}
} // namespace aquarius