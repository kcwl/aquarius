#include "schema.h"
#include <iostream>

namespace
{
	std::map<std::string, std::string> structure_keyword = { { "message", "class" },
															 { "struct", "struct" },
															 { "enum", "enum class" } };

	std::set<std::string> keywords = {
		"int32", "uint32", "sint32", "int64",  "uint64", "sint64", "fixed32", "fixed64",
		"bool",	 "float",  "double", "string", "bytes",	 "date",   "times",	  "datetime"
	};

	std::string from_type_string(const std::string& target)
	{
		auto iter = keywords.find(target);
		if (iter == keywords.end())
		{
			return target;
		}

		return *iter;
	}
} // namespace

namespace aquarius
{
	namespace lazytool
	{
		std::string schema_base::log(const std::string& func_name, const std::string& str)
		{
			return std::format("[{}] {} at [row:{}, column:{}]", func_name, str, row_, column_);
		}

		void schema_base::put_custom_type(const std::string& type)
		{
			custom_types_.insert(type);
		}

		bool schema_base::check_custom_type(const std::string& type)
		{
			return custom_types_.find(type) != custom_types_.end();
		}

		schema::schema()
			: fields_()
		{
			invokes_.insert({ "message", [this](std::fstream& ifs, const std::string& protocol)
							  {
								  auto message = std::make_shared<message_field>(struct_type::message);

								  auto res = parse_message_field(ifs, message, protocol);

								  if (!res)
									  throw std::runtime_error(std::format("protocol parse error! row: {}, column: {}",
																		   row_ + 1, column_ + 1));
								  fields_.push_back(message);
							  } });

			invokes_.insert({ "struct", [&](std::fstream& ifs, const std::string&)
							  {
								  auto field = std::make_shared<field_base>(struct_type::structure);

								  auto res = parse_data_field(ifs, field);

								  if (!res)
									  throw std::runtime_error(std::format("struct parse error! row: {}, column: {}",
																		   row_ + 1, column_ + 1));

								  fields_.push_back(field);

								  put_custom_type(field->name());
							  } });

			invokes_.insert({ "enum", [&](std::fstream& ifs, const std::string&)
							  {
								  auto enums = std::make_shared<field_base>(struct_type::enumture);

								  auto res = parse_data_field(ifs, enums, false, false);

								  if (!res)
									  throw std::runtime_error(
										  std::format("enum parse error! row: {}, column: {}", row_ + 1, column_));
								  fields_.push_back(enums);
							  } });
		}

		std::vector<std::shared_ptr<field_base>> schema::run(const std::string& file_path, const std::string& protocol)
		{
			std::fstream ifs(file_path, std::ios::in | std::ios::binary);
			if (!ifs.is_open())
				return {};

			while (!ifs.eof())
			{
				auto result = read_value<token::strict, ' '>(ifs);

				if (!result.has_value())
					continue;

				try
				{
					auto f = invoke(*result);
					f(ifs, protocol);
				}
				catch (std::exception& e)
				{
					std::cout << e.what() << std::endl;
					return {};
				}
			}

			return fields_;
		}

		bool schema::parse_data_field(std::fstream& ifs, std::shared_ptr<field_base> field_ptr, bool anonymous,
									  bool has_key)
		{
			if (!anonymous)
			{
				auto result = read_value<token::standard, '{'>(ifs);

				if (!result.has_value())
				{
					return false;
				}

				field_ptr->set_name(*result);
			}

			while (!ifs.eof())
			{
				std::expected<std::string, bool> result{};
				if (has_key)
				{
					result = read_value<token::standard, ' '>(ifs);

					if (!result.has_value())
					{
						return result.error();
					}

					if (*result == "repeated")
					{
						auto t = read_value<token::standard, ' '>(ifs);
						if (!t.has_value())
						{
							return false;
						}

						*result = "std::vector<" + *t + ">";
					}
					else if (*result == "map")
					{
						auto key = read_value<token::standard, ','>(ifs);
						if (!key.has_value())
						{
							return false;
						}
						auto value = read_value<token::strict, ' '>(ifs);
						if (!value.has_value())
						{
							return false;
						}

						*result = "std::map<" + *key + ", " + *value + ">";
					}
				}

				auto v = read_value<token::standard, ';'>(ifs);
				if (!v.has_value())
				{
					return v.error();
				}

				field_ptr->add_field(*result, *v);
			}

			return true;
		}

		bool schema::parse_router(std::fstream& ifs, std::shared_ptr<message_field> field_ptr)
		{
			auto result = read_value<token::standard, '>'>(ifs);

			if (!result.has_value())
			{
				return false;
			}

			result = read_value<token::loose, '{'>(ifs);

			if (!result.has_value())
			{
				return false;
			}

			field_ptr->set_router(*result);
			return true;
		}

		bool schema::parse_message_field(std::fstream& ifs, std::shared_ptr<message_field> field_ptr,
										 const std::string& protocol)
		{
			auto result = read_value<token::standard, '-'>(ifs);
			if (!result.has_value())
			{
				return false;
			}

			field_ptr->set_name(*result);

			field_ptr->set_protocol_name(*result);

			if (!parse_router(ifs, field_ptr))
			{
				return false;
			}

			auto f = [&]<std::size_t... I>(std::index_sequence<I...>)
			{ return (read_protocol<I>(ifs, field_ptr) && ...); };

			return f(std::make_index_sequence<message_field::message_part>());
		}

		bool schema::check_type(const std::string& value)
		{
			return keywords.find(value) != keywords.end();
		}

		auto schema::invoke(const std::string& struct_name) -> func_t
		{
			auto iter = invokes_.find(struct_name);
			if (iter == invokes_.end())
				throw std::runtime_error(
					log("main",
						std::format("syntax error [{}]! missing protocol or struct or enum keyword!", struct_name)));

			return iter->second;
		}
	} // namespace lazytool
} // namespace aquarius