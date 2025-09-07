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

		template <char... end>
		std::expected<std::string, parse_error> read_value(std::fstream& ifs, std::size_t& column, std::size_t& row,
														   token t = token::value)
		{
			std::string value{};
			auto offset = ifs.tellg();

			int c{};

			while (!ifs.eof())
			{
				column++;

				c = ifs.get();

				if (c == -1)
					return std::unexpected(parse_error::eof);

				if (std::isalnum(c) || (t == token::value && (c == '_' || std::isalnum(c))) ||
					(t == token::key && c == '-') || (t == token::path && ((c != end) && ...)))
				{
					value += static_cast<char>(c);

					continue;
				}

				if (((c == end) || ...))
				{
					return value;
				}

				if (c != '\t' && c != '\n' && c != '\r' && c != ' ')
				{
					break;
				}

				if (c == '\n')
				{
					column = 0;
					row++;
				}
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

				if (c != ' ' && c != '\t' && c != '\n' && c != '\r')
				{
					break;
				}

				if (c == '\n')
				{
					column++;
					row = 0;
				}

				row++;
			}

			auto length = ifs.tellg() - offset;

			ifs.seekg(-length, std::ios_base::cur);

			return std::unexpected(parse_error::eof);
		}
	} // namespace virgo
} // namespace aquarius
