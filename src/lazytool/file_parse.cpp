#include "file_parse.h"
#include "message_parse.h"
#include <format>
#include "structure_parse.h"
#include "enumture_parse.h"
#include <iostream>
#include "struct_type.hpp"
#include "router_parse.h"
#include "protocol_parse.h"
#include "normal_message_parse.h"


namespace
{
	using namespace aquarius::lazytool;

	std::string from_struct_type_string(struct_type type)
	{
		switch (type)
		{
		case struct_type::none:
			return "none";
		case struct_type::structure:
			return "struct";
		case struct_type::message:
			return "class";
		case struct_type::enumture:
			return "enum class";
		case struct_type::router:
			return "router";
		case struct_type::protocol:
			return "protocol";
		case struct_type::domin:
			return "domain";
		}

		return {};
	}
} // namespace

namespace aquarius
{
	namespace lazytool
	{
		file_parser::file_parser()
			: keywords_()
			, registor_()
			, column_(0)
			, row_(0)
		{
			registor_.regist("protocol",
							 [this](std::ifstream& ifs)
							 {
								 auto proto_ptr = std::make_shared<message_parse>();
								 auto res = proto_ptr->visit(ifs, column_, row_);
								 if (res != parser::parse_error::success)
									 throw std::runtime_error(std::format("protocol parse error! row: {}, column: {}",
																		  row_ + 1, column_ + 1));
								 keywords_.push_back(proto_ptr);
							 });

			registor_.regist("message",
							 [this](std::ifstream& ifs)
							 {
								 auto proto_ptr = std::make_shared<normal_message_parse>();
								 auto res = proto_ptr->visit(ifs, column_, row_);
								 if (res != parser::parse_error::success)
									 throw std::runtime_error(std::format("protocol parse error! row: {}, column: {}",
																		  row_ + 1, column_ + 1));
								 keywords_.push_back(proto_ptr);
							 });

			registor_.regist("struct",
							 [&](std::ifstream& ifs)
							 {
								 auto struct_ptr = std::make_shared<structure_parse>();

								 auto res = struct_ptr->visit(ifs, column_, row_);

								 if (res != parser::parse_error::success)
									 throw std::runtime_error(
										 std::format("struct parse error! row: {}, column: {}", row_ + 1, column_ + 1));
								 keywords_.push_back(struct_ptr);
							 });

			registor_.regist("enum",
							 [&](std::ifstream& ifs)
							 {
								 auto enum_ptr = std::make_shared<enumture_parse>();

								 auto res = enum_ptr->visit(ifs, column_, row_);

								 if (res != parser::parse_error::success)
									 throw std::runtime_error(
										 std::format("enum parse error! row: {}, column: {}", row_ + 1, column_));
								 keywords_.push_back(enum_ptr);
							 });
		}

		bool file_parser::read_file(const std::string& file_path)
		{
			std::ifstream ifs(file_path);
			if (!ifs.is_open())
				return false;

			parser pr{};

			while (!ifs.eof())
			{
				auto value = pr.read_value<parser::token::key, ' '>(ifs, column_, row_);

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

		std::ostream& operator<<(std::ostream& os, const file_parser& pr)
		{
			for (auto& s : pr.keywords_)
			{
				os << from_struct_type_string(s->struct_type_) << " " << s->name_ << std::endl;
				os << "{\n";

				if (s->struct_type_ == struct_type::enumture)
				{
					for (auto& m : std::dynamic_pointer_cast<enumture_parse>(s)->values_)
					{
						os << "\t" << m.first << "," << std::endl;
					}
				}
				else if (s->struct_type_ == struct_type::structure)
				{
					for (auto& m : std::dynamic_pointer_cast<structure_parse>(s)->values_)
					{
						os << "\t" << m.first << " " << m.second << ";" << std::endl;
					}
				}
				else if (s->struct_type_ == struct_type::message)
				{
					auto msg_ptr = std::dynamic_pointer_cast<message_parse>(s);

					os << *msg_ptr->router_ptr_ << *msg_ptr->request_ptr_ << *msg_ptr->response_ptr_;
				}

				os << "};" << std::endl;
			}

			return os;
		}

	} // namespace lazytool
} // namespace aquarius
