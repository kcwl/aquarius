#include "file_parse.h"
#include <format>
#include <iostream>
#include "parse_helper.hpp"
#include <set>
#include "custom_type.h"


namespace
{
	using namespace aquarius::lazytool;


	std::string from_struct_type_string(struct_type type)
	{
		switch (type)
		{
			case struct_type::structure:
				return "struct";
			case struct_type::message:
				return "message";
			case struct_type::enumture:
				return "enum class";
		}

		return "none";
	}


	

	static const std::set<std::string> router_protocol = { "tcp", "http" };

	static const std::map<std::string, std::vector<std::string>> router_mode =
	{
		{"tcp", {}},
		{"http", {"get", "post"}}
	};

	parse_error parse_data_field(std::ifstream& ifs, std::size_t& column, std::size_t& row, data_field& field, bool anonymous = false, bool has_key = true)
	{
		if (!anonymous)
		{
			char end;
			auto name = read_value<token::value, '{'>(ifs, column, row, end);
			if (name.empty())
				return parse_error::syntax;

			if (get_custom_type(name))
			{
				return parse_error::multi_define;
			}

			field.set_name(name);
		}

		while (!ifs.eof())
		{
			auto res = seek<'}'>(ifs, column, row);
			if (res == parse_error::success)
				return res;

			std::string k{};
			char end;
			if (has_key)
			{
				k = read_value<token::key, ' '>(ifs, column, row, end);
				if (k.empty())
					return parse_error::syntax;
			}

			auto v = read_value<token::value, ';'>(ifs, column, row, end);
			if (v.empty())
				return parse_error::syntax;

			field.add_field(k, v);
		}

		return ifs.eof() ? parse_error::eof : parse_error::syntax;
	}

	parse_error parse_router_field(std::ifstream& ifs, std::size_t& column, std::size_t& row, router_field& field)
	{
		char end;
		auto value = read_value<token::key, '-', '['>(ifs, column, row, end);

		if (end == '[')
		{
			if (!field.set_protocol(value))
				return parse_error::routers;

			value = read_value<token::key, ']'>(ifs, column, row, end);
			if (!field.set_method(value))
				return parse_error::routers_mode;

			auto result = seek<'-'>(ifs, column, row);
			if (result != parse_error::success)
				return parse_error::syntax;
			result = seek<'>'>(ifs, column, row);
			if (result != parse_error::success)
				return parse_error::syntax;
		}
		else
		{
			auto result = seek<'>'>(ifs, column, row);
			if (result != parse_error::success)
				return parse_error::syntax;

			if (!field.set_protocol(value))
				return parse_error::routers;
		}

		value = read_value<token::path, '{'>(ifs, column, row, end);
		if (value.empty())
			return parse_error::routers_value;
		field.set_router(value);
		return parse_error::success;
	}

	parse_error parse_message_field(std::ifstream& ifs, std::size_t& column, std::size_t& row, message_field& field)
	{
		char end;
		auto value = read_value<token::value, ':'>(ifs, column, row, end);
		if (value.empty())
			return parse_error::syntax;

		field.set_name(value);

		auto result = parse_router_field(ifs, column, row, field);
		if (result != parse_error::success)
			return result;

		int part = 2;
		while (part--)
		{
			value = read_value<token::key, '{'>(ifs, column, row, end);
			if (value == "request")
			{
				result = parse_data_field(ifs, column, row, *field.request(), true);
			}
			else if (value == "response")
			{
				result = parse_data_field(ifs, column, row, *field.response(), true);
			}
			else
			{
				return parse_error::keyword;
			}
		}

		result = seek<'}'>(ifs, column, row);

		return result;
	}
} // namespace

namespace aquarius
{
	namespace lazytool
	{
		data_field::data_field(struct_type s)
			: type_(s)
		{

		}

		bool data_field::add_field(const std::string& type, const std::string& value)
		{
			auto iter = std::find_if(fields_.begin(), fields_.end(),
									 [&] (const auto& field) { return field.second == value; });

			if (iter != fields_.end())
			{
				std::cout << "Already have this field" << std::endl;

				return false;
			}

			fields_.emplace_back(type, value);

			return false;
		}

		std::string data_field::name() const
		{
			return name_;
		}

		struct_type data_field::type() const
		{
			return type_;
		}

		void data_field::set_name(const std::string& name)
		{
			name_ = name;
		}

		std::vector<std::pair<std::string, std::string>> data_field::fields() const
		{
			return fields_;
		}

		std::ostream& operator<<(std::ostream& os, const data_field& field)
		{
			os << from_struct_type_string(field.type()) << ": " << field.name() << std::endl << "{" << std::endl;
			for (const auto& ff : field.fields())
			{
				os << "\t" << ff.first << " " << ff.second << std::endl;
			}
			os << "}" << std::endl;

			return os;
		}

		bool router_field::set_protocol(const std::string& protocol)
		{
			if (!check_protocol_type(protocol))
				return false;

			protocol_ = protocol;

			return true;
		}

		std::string router_field::protocol() const
		{
			return protocol_;
		}

		bool router_field::set_method(const std::string& method)
		{
			if (!check_mode(this->protocol(), method))
				return false;

			method_ = method;

			return true;
		}

		std::string router_field::method() const
		{
			return method_;
		}

		void router_field::set_router(const std::string& router)
		{
			router_ = router;
		}

		std::string router_field::router() const
		{
			return router_;
		}

		bool router_field::check_protocol_type(const std::string& protocol_type)
		{
			return router_protocol.find(protocol_type) != router_protocol.end();
		}

		bool router_field::check_mode(const std::string& protocol, const std::string& mode)
		{
			auto iter = router_mode.find(protocol);
			if (iter == router_mode.end())
			{
				return false;
			}
			auto& modes = iter->second;
			return std::find(modes.begin(), modes.end(), mode) != modes.end();
		}

		message_field::message_field()
			: data_field(struct_type::message)
			, request_ptr_(new data_field(struct_type::message))
			, response_(new data_field(struct_type::message))
		{

		}

		void message_field::set_request(std::shared_ptr<data_field> request)
		{
			request_ptr_ = request;
		}

		std::shared_ptr<data_field> message_field::request()
		{
			return request_ptr_;
		}

		void message_field::set_response(std::shared_ptr<data_field> response)
		{
			response_ = response;
		}

		std::shared_ptr<data_field> message_field::response()
		{
			return response_;
		}

		std::ostream& operator<<(std::ostream& os, message_field& field)
		{
			os << "message: router->" << field.router() << " " << field.protocol() << " " << field.method() << std::endl << "{" << std::endl;
			os << "\trequest:" << std::endl << field.request()->name() << std::endl << "{" << std::endl;
			for (const auto& ff : field.request()->fields())
			{
				os << "\t\t" << ff.first << " " << ff.second << std::endl;
			}
			os << "\t}" << std::endl;
			os << "\tresponse:" << std::endl << field.response()->name() << std::endl << "{" << std::endl;
			for (const auto& ff : field.response()->fields())
			{
				os << "\t\t" << ff.first << " " << ff.second << std::endl;
			}
			os << "\t}"<<std::endl;
			os << "}" << std::endl;
			return os;
		}

		proto_parser::proto_parser()
			: fields_()
			, registor_()
			, column_(0)
			, row_(0)
		{
			registor_.regist("message",
							 [this] (std::ifstream& ifs)
							 {
								 auto message = std::make_shared<message_field>();

								 auto res = parse_message_field(ifs, column_, row_, *message);


								 if (res != parse_error::success)
									 throw std::runtime_error(std::format("protocol parse error! row: {}, column: {}",
																		  row_ + 1, column_ + 1));
								 fields_.push_back(message);
							 });

			registor_.regist("struct",
							 [&] (std::ifstream& ifs)
							 {
								 auto field = std::make_shared<data_field>(struct_type::structure);

								 auto res = parse_data_field(ifs, column_, row_, *field);

								 if (res != parse_error::success)
									 throw std::runtime_error(
										 std::format("struct parse error! row: {}, column: {}", row_ + 1, column_ + 1));

								 fields_.push_back(field);

								 put_custom_type(field->name(), field);
							 });

			registor_.regist("enum",
							 [&] (std::ifstream& ifs)
							 {
								 auto enums = std::make_shared<data_field>(struct_type::enumture);

								 auto res = parse_data_field(ifs, column_, row_, *enums, false, false);

								 if (res != parse_error::success)
									 throw std::runtime_error(
										 std::format("enum parse error! row: {}, column: {}", row_ + 1, column_));
								 fields_.push_back(enums);
							 });
		}

		bool proto_parser::read_file(const std::string& file_path)
		{
			std::ifstream ifs(file_path);
			if (!ifs.is_open())
				return false;

			char end;
			while (!ifs.eof())
			{
				auto value = read_value<token::key, ' '>(ifs, column_, row_, end);

				if (value.empty())
					continue;

				try
				{
					auto f = registor_.invoke(value, column_, row_);
					f(ifs);
				}
				catch (std::exception& e)
				{
					std::cout << e.what() << std::endl;
					return false;
				}
			}

			return true;
		}

		std::ostream& operator<<(std::ostream& os, const proto_parser& pr)
		{
			for (auto& s : pr.fields_)
			{

				if (s->type() == struct_type::message)
				{
					os << *std::static_pointer_cast<message_field>(s);
				}
				else
				{
					os << *s;
				}
			}

			return os;
		}


	} // namespace lazytool
} // namespace aquarius
