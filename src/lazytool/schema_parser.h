#pragma once
#include "field.h"
#include <format>
#include <fstream>
#include <set>
#include <vector>

namespace aquarius
{
	namespace lazytool
	{
		class schema_parser
		{
		public:
			virtual std::vector<std::shared_ptr<field>> run(const std::string& file_path,
															const std::string& protocol) = 0;

		protected:
			std::string log(const std::string& func_name, const std::string& str, std::size_t column, std::size_t row);

			enum token
			{
				name,
				path,
				domin,
			};

			void skip_space(std::fstream& ifs, std::size_t& column, std::size_t& row)
			{
				while (!ifs.eof())
				{
					auto c = ifs.peek();

					if (!std::isspace(c))
						break;

					if (c == '\n')
					{
						column = 0;
						row++;
					}

					ifs.get();
				}
			}

			template <token Token, char... end>
			std::string read_value(std::fstream& ifs, char& ends)
			{
				skip_space(ifs, column_, row_);

				std::string value{};

				while (!ifs.eof())
				{
					ends = static_cast<char>(ifs.get());

					column_++;
					if (((ends == end) || ...))
					{
						return value;
					}

					if (std::isspace(ends))
					{
						if (ends == '\n')
						{
							column_ = 0;
							row_++;
						}

						continue;
					}

					if constexpr (Token == token::name)
					{
						if (ends != '_' && !std::isalnum(ends))
						{
							break;
						}
					}
					else if constexpr (Token == token::domin)
					{
						if (!std::isalnum(ends) && ends != '-' && ends != '<' && ends != '>')
						{
							if constexpr (Token == token::domin)
							{
								ifs.seekg(-static_cast<int64_t>(value.size()), std::ios::cur);
							}

							break;
						}
					}

					value += ends;
				}

				return value;
			}

			template <char sp, bool skip = true>
			bool seek(std::fstream& ifs)
			{
				if constexpr (skip)
				{
					skip_space(ifs, column_, row_);
				}

				auto c = ifs.peek();

				return static_cast<char>(c) == sp;
			}

			void put_custom_type(const std::string& type);

			bool check_custom_type(const std::string& type);

		protected:
			std::size_t column_;

			std::size_t row_;

		private:
			std::set<std::string> custom_types_;
		};
	} // namespace lazytool
} // namespace aquarius