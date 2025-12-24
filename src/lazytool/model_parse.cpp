#include "model_parse.h"
#include "parse_helper.hpp"

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
					ifs.get();
					break;
				}

				field.name = read_value<token::value, ' ', ';'>(ifs, column_, row_, end);

				while (end != ';')
				{
					auto t = read_value<token::value, '@', '^', '$'>(ifs, column_, row_, end);

					if (end != '@' && end != '^' && end != '$')
						return false;

					t = end;

					value = read_value<token::value, ' ', ';'>(ifs, column_, row_, end);

					if (value.empty())
						return false;

					auto beg = *t.begin();

					if (beg == '@')
					{
						field.attrs.push_back("key<"+value + ">");
					}
					else if (beg == '^')
					{
						field.attrs.push_back("index<" + value + ">");
					}
					else if (beg == '$')
					{
						field.attrs.push_back("attr<" + value + ">");
					}
					else
					{
						return false;
					}
				}
			}

			return true;
		}
	} // namespace lazytool
} // namespace aquarius