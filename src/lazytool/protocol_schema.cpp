#include "protocol_schema.h"
#include <iostream>

namespace
{
	std::map<std::string, std::string> structure_keyword = { { "message", "class" },
															 { "struct", "struct" },
															 { "enum", "enum class" } };

	std::set<std::string> keywords = { "bool", "uint32", "int32", "uint64", "float", "double", "string", "bytes" };

	std::set<std::string> model_keywords = { "float",  "double",   "date", "timestamp", "integer",
											 "string", "datetime", "time", "binary" };

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
		protocol_schema::protocol_schema()
			: fields_()
		{
			invokes_.insert({ "message", [this](std::fstream& ifs, const std::string& protocol)
							  {
								  auto message = std::make_shared<message_field>(struct_type::message);

								  message->set_protocol(protocol);

								  auto res = parse_message_field(ifs, message);

								  if (!res)
									  throw std::runtime_error(std::format("protocol parse error! row: {}, column: {}",
																		   row_ + 1, column_ + 1));
								  fields_.push_back(message);
								  ifs.get();
							  } });

			invokes_.insert({ "struct", [&](std::fstream& ifs,const std::string&)
							  {
								  auto field = std::make_shared<data_field>(struct_type::structure);

								  auto res = parse_data_field(ifs, field);

								  if (!res)
									  throw std::runtime_error(std::format("struct parse error! row: {}, column: {}",
																		   row_ + 1, column_ + 1));

								  fields_.push_back(field);

								  put_custom_type(field->name());

								  ifs.get();
							  } });

			invokes_.insert({ "enum", [&](std::fstream& ifs,const std::string&)
							  {
								  auto enums = std::make_shared<data_field>(struct_type::enumture);

								  auto res = parse_data_field(ifs, enums, false, false);

								  if (!res)
									  throw std::runtime_error(
										  std::format("enum parse error! row: {}, column: {}", row_ + 1, column_));
								  fields_.push_back(enums);

								  ifs.get();
							  } });

			invokes_.insert({ "model", [&](std::fstream& ifs,const std::string&)
							  {
								  auto models = std::make_shared<data_field>(struct_type::model);

								  auto res = parse_model(ifs, models);

								  if (!res)
									  throw std::runtime_error(
										  std::format("model parse error! row: {}, column: {}", row_ + 1, column_));
								  fields_.push_back(models);

								  ifs.get();
							  } });
		}

		std::vector<std::shared_ptr<field>> protocol_schema::run(const std::string& file_path,
																 const std::string& protocol)
		{
			std::fstream ifs(file_path, std::ios::in | std::ios::binary);
			if (!ifs.is_open())
				return {};

			char end;
			while (!ifs.eof())
			{
				auto value = read_value<token::name, ' '>(ifs, end);

				if (value.empty())
					continue;

				try
				{
					auto f = invoke(value);
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

		bool protocol_schema::parse_data_field(std::fstream& ifs, std::shared_ptr<data_field> field_ptr, bool anonymous,
											   bool has_key)
		{
			if (!anonymous)
			{
				char end;
				auto name = read_value<token::name, '{'>(ifs, end);
				if (name.empty())
					return false;

				field_ptr->set_name(name);
			}

			while (!ifs.eof())
			{
				auto res = seek<'}'>(ifs);
				if (res)
					return true;

				std::string k{};
				char end;
				if (has_key)
				{
					k = read_value<token::name, ' '>(ifs, end);

					if (end == '{')
					{
						continue;
					}

					if (k.empty())
						return false;

					if (k == "repeated")
					{
						auto t = read_value<token::name, ' '>(ifs, end);
						if (end != ' ')
							return false;

						k = "std::vector<" + t + ">";
					}
				}

				auto v = read_value<token::name, ';'>(ifs, end);
				if (v.empty())
					return false;

				field_ptr->add_field(k, v);

				ifs.get();
			}

			return true;
		}

		bool protocol_schema::parse_router_field(std::fstream& ifs, std::shared_ptr<message_field> field_ptr)
		{
			char end;
			if (!seek<'-'>(ifs))
			{
				return false;
			}

			ifs.get();

			if (!seek<'>'>(ifs))
			{
				return false;
			}

			ifs.get();

			auto value = read_value<token::path, '{'>(ifs, end);
			if (value.empty())
				return false;
			field_ptr->set_router(value);
			return true;
		}

		bool protocol_schema::parse_message_field(std::fstream& ifs, std::shared_ptr<message_field> field_ptr)
		{
			char end;
			auto value = read_value<token::name, ' '>(ifs, end);
			if (end == '[')
			{
				auto method = read_value<token::name, ']'>(ifs, end);
				if (end != ']')
				{
					return false;
				}

				field_ptr->method(method);

				ifs.get();
			}

			field_ptr->set_name(value);

			auto result = parse_router_field(ifs, field_ptr);
			if (!result)
				return false;

			ifs.get();

			int part = 2;
			while (part--)
			{
				value = read_value<token::name, '{'>(ifs, end);

				if (value == "request")
				{
					field_ptr->request()->set_name(field_ptr->name());
					result = parse_data_field(ifs, field_ptr->request(), true);
				}
				else if (value == "response")
				{
					field_ptr->response()->set_name(field_ptr->name());
					result = parse_data_field(ifs, field_ptr->response(), true);
				}
				else
				{
					std::cout << "message only include request and response\n";
					return false;
				}

				if (!result)
				{
					return false;
				}

				ifs.get();
			}

			if (!seek<'}'>(ifs))
			{
				return false;
			}

			return result;
		}

		bool protocol_schema::parse_model(std::fstream& ifs, std::shared_ptr<data_field> field_ptr)
		{
			char end{};

			auto value = read_value<token::name, ' '>(ifs, end);

			if (value.empty())
				return false;

			if (value != "model")
				return false;

			field_ptr->name() = read_value<token::name, '{'>(ifs, end);

			if (field_ptr->name().empty())
				return false;

			while (!ifs.eof())
			{
				auto type = read_value<token::name, ' '>(ifs, end);

				if (end == '}')
				{
					return true;
				}

				if (!check_type(type))
				{
					std::cout << "error type[" << type << "]" << std::endl;
					return false;
				}

				auto value = read_value<token::name, ' ', ';'>(ifs, end);

				field_ptr->add_field(type, value);
			}

			return true;
		}

		bool protocol_schema::check_type(const std::string& value)
		{
			return model_keywords.find(value) != model_keywords.end();
		}

		auto protocol_schema::invoke(const std::string& struct_name) -> func_t
		{
			auto iter = invokes_.find(struct_name);
			if (iter == invokes_.end())
				throw std::runtime_error(log(
					"main", std::format("syntax error [{}]! missing protocol or struct or enum keyword!", struct_name),
					column_, row_));

			return iter->second;
		}
	} // namespace lazytool
} // namespace aquarius