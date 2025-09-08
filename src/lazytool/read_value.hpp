#pragma once
#include <format>
#include <fstream>
#include <string>
#include "log.h"
#include <expected>

namespace aquarius
{
	namespace virgo
	{
		enum token
		{
			value,
			key,
			path
		};

		template <token Token, char... end>
		std::expected<std::string, parse_error> read_value(std::fstream& ifs, std::size_t& column, std::size_t& row)
		{
			std::string value{};
			auto offset = ifs.tellg();

			int c{};

			while (!ifs.eof())
			{
				c = ifs.peek();

				if (c == -1)
					return std::unexpected(parse_error::success);

				column++;

				if (((c == end) || ...))
				{
					ifs.get();
					return value;
				}

				if (std::isspace(c))
				{
					if (c == '\n')
					{
						column = 0;
						row++;
					}

					ifs.get();

					continue;
				}

				if constexpr (Token == token::value)
				{
					if (c != '_' && !std::isalnum(c))
						break;
				}
				else if constexpr (Token == token::key)
				{
					if (!std::isalpha(c) && c != '-')
						break;
				}
				else if constexpr (Token == token::path)
				{
				}

				value += static_cast<char>(ifs.get());
			}

			auto length = ifs.tellg() - offset;

			ifs.seekg(-length, std::ios_base::cur);

			return std::unexpected(parse_error::eof);
		}

		template <char sp>
		std::expected<bool, parse_error> seek(std::fstream& ifs, std::size_t& column, std::size_t& row)
		{
			auto offset = ifs.tellg();

			while (!ifs.eof())
			{
				auto c = ifs.get();

				if (c == sp)
				{
					return true;
				}

				if (!std::isspace(c))
				{
					break;
				}

				if (c == '\n')
				{
					column = 0;
					row++;
				}

				column++;
			}

			auto length = ifs.tellg() - offset;

			ifs.seekg(-length, std::ios_base::cur);

			return std::unexpected(parse_error::eof);
		}
	} // namespace virgo
} // namespace aquarius
