#include "model_parse.h"
#include "parse_helper.hpp"
#include <iostream>

namespace aquarius
{
	namespace lazytool
	{

		bool model_parse::read_file(const std::string& file_path)
		{
			std::ifstream ifs(file_path);
			if (!ifs.is_open())
				return false;

			while (!ifs.eof())
			{
				auto model_field_ptr = std::make_shared<model_field>();

				if (!parse_model(ifs, model_field_ptr))
					continue;

				models_.push_back(model_field_ptr);
			}

			return true;
		}

		bool model_parse::parse_model(std::ifstream& ifs, std::shared_ptr<model_field> model_field_ptr)
		{
			char end{};

			auto value = read_value<token::key, ' '>(ifs, column_, row_, end);

			if (value.empty())
				return false;

			if (value != "model")
				return false;

			model_field_ptr->name = read_value<token::value, '-'>(ifs, column_, row_, end);

			if (model_field_ptr->name.empty())
				return false;

			auto error = seek<'>'>(ifs, column_, row_);

			if (error != parse_error::success)
				return false;

			model_field_ptr->db_languase = read_value<token::value, '{'>(ifs, column_, row_, end);

			if (model_field_ptr->db_languase.empty())
				return false;

			while (!ifs.eof())
			{
				model_field_ptr->fields.push_back({});

				auto& field = model_field_ptr->fields.back();

				field.type = read_value<token::key, ' '>(ifs, column_, row_, end);

				if (end == '}')
				{
					return true;
				}

				if (!check_type(field.type))
				{
					std::cout << "error type! value:" << field.type << std::endl;
					return false;
				}

				field.name = read_value<token::value, ' ', ';'>(ifs, column_, row_, end);

				while (end != ';')
				{
					auto t = read_value<token::value, '$'>(ifs, column_, row_, end);

					if (end != '$')
						return false;

					t = end;

					if (end == '}')
						return true;

					value = read_value<token::value, ' ', ';'>(ifs, column_, row_, end);

					if (value.empty())
					{
						std::cout << "value empty:" << std::endl;

						return false;
					}

					auto beg = *t.begin();

					if (beg != '$')
					{
						std::cout << "attribute key-word error! value:" << beg << std::endl;

						return false;
					}
						
					if (!check_attribute(value))
					{
						std::cout << "check attribute error! value:" << value << std::endl;

						return false;
					}
						
					field.attrs.push_back(value);
				}
			}

			return true;
		}

		bool model_parse::check_attribute(const std::string& value)
		{
			auto f = [&](const auto&... args) { return ((value == args) || ...); };

			return f("PK", "UN", "AI", "AT", "DF", "NN");
		}

		bool model_parse::check_type(const std::string& value)
		{
			auto sp = value.find_first_of("<");

			if (sp == std::string::npos)
			{
				if (value != "float" && value != "double" && value != "date" && value != "timestamp")
					return false;
			}
			else
			{
				auto prefix = value.substr(0, sp);

				auto f = [&](const auto&... args) { return ((prefix == args) || ...); };

				if (!f("integer", "char", "varchar", "decimal", "bit", "datetime", "time", "binary", "varbinary"))
					return false;

				auto pos = value.find_last_of(">");

				if (pos == std::string::npos)
					return false;

				auto suffix = value.substr(sp + 1, pos - sp - 1);

				for (auto& c : suffix)
				{
					if (!std::isdigit(c))
						return false;
				}
			}

			return true;
		}
	} // namespace lazytool
} // namespace aquarius