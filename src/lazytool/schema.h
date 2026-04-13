#pragma once
#include "field.h"
#include <expected>
#include <format>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <set>
#include <vector>

namespace aquarius
{
	namespace lazytool
	{
		class schema_base
		{
		public:
			virtual std::vector<std::shared_ptr<field_base>> run(const std::string& file_path,
																 const std::string& protocol) = 0;

		protected:
			std::string log(const std::string& func_name, const std::string& str);

			enum token
			{
				strict,
				standard,
				loose,
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
			auto read_value(std::fstream& ifs) -> std::expected<std::string, bool>
			{
				char ends{};
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

					if constexpr (Token == token::strict)
					{
						if (!std::isalpha(ends))
						{
							if (ends == '}')
							{
								return std::unexpected(true);
							}

							break;
						}
					}

					if constexpr (Token == token::standard)
					{
						if (ends != '_' && !std::isalnum(ends))
						{
							if (ends == '}')
							{
								return std::unexpected(true);
							}

							break;
						}
					}

					value += ends;
				}

				return std::unexpected(false);
			}

			void put_custom_type(const std::string& type);

			bool check_custom_type(const std::string& type);

		protected:
			std::size_t column_;

			std::size_t row_;

		private:
			std::set<std::string> custom_types_;
		};

		class schema : public schema_base
		{
			using func_t = std::function<void(std::fstream&, const std::string&)>;

		public:
			schema();

		public:
			virtual std::vector<std::shared_ptr<field_base>> run(const std::string& file_path,
																 const std::string& protocol) override;

		private:
			bool parse_data_field(std::fstream& ifs, std::shared_ptr<field_base> field, bool anonymous = false,
								  bool has_key = true);

			bool parse_router(std::fstream& ifs, std::shared_ptr<message_field> field_ptr);

			bool parse_message_field(std::fstream& ifs, std::shared_ptr<message_field> field_ptr,
									 const std::string& protocol);

			template <std::size_t I>
			bool read_protocol(std::fstream& ifs, std::shared_ptr<message_field> field_ptr)
			{
				auto value = read_value<token::strict, '{'>(ifs);

				if (!value.has_value() || (value != "request" && value != "response"))
				{
					std::cout << "message only include request and response\n";
					return false;
				}

				return parse_data_field(ifs, field_ptr->get<I>(), true);
			}

			bool check_type(const std::string& value);

			func_t invoke(const std::string& struct_name);

		private:
			std::vector<std::shared_ptr<field_base>> fields_;

			std::map<std::string, func_t> invokes_;
		};
	} // namespace lazytool
} // namespace aquarius