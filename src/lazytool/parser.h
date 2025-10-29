#pragma once
#include "parse_base.h"
#include "struct_type.hpp"

namespace aquarius
{
	namespace lazytool
	{
		class parser : public parse_base
		{
		public:
			parser();

			parser(struct_type t);

			virtual ~parser() = default;

		public:
			enum token
			{
				value,
				key,
				path,
				domin,
			};

			virtual parse_error visit(std::ifstream&, std::size_t&, std::size_t&) override
			{
				return parse_error::success;
			}

			template <token Token, char... end>
			std::string read_value(std::ifstream& ifs, std::size_t& column, std::size_t& row)
			{
				std::string value{};

				int c{};
				while (!ifs.eof())
				{
					c = ifs.peek();
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
					else if constexpr (Token == token::key || Token == token::domin)
					{
						if (!std::isalnum(c) && c != '-')
						{
							if constexpr (Token == token::domin)
							{
								ifs.seekg(-static_cast<int64_t>(value.size()), std::ios::cur);
							}

							break;
						}
					}

					value += static_cast<char>(ifs.get());
				}

				return {};
			}

			template <char sp>
			parse_error seek(std::ifstream& ifs, std::size_t& column, std::size_t& row)
			{
				while (!ifs.eof())
				{
					auto c = ifs.peek();
					if (c == sp)
					{
						ifs.get();
						return parse_error::success;
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
					ifs.get();
				}

				return ifs.eof() ? parse_error::eof : parse_error::syntax;
			}

			struct_type structtype() const;

		public:
			struct_type struct_type_;
		};
	} // namespace lazytool
} // namespace aquarius
