#include "parse.h"
#include "read_value.hpp"
#include <algorithm>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>

namespace aquarius
{
	namespace virgo
	{
		parse::parse()
			: pros_()
			, regist_()
			, row_(0)
			, column_(0)
		{
			regist_.regist("protocol",
						   [this](std::fstream& ifs)
						   {
							   auto proto_ptr = std::make_shared<protocol>();
							   proto_ptr->parse(ifs, column_, row_, proto_types);
							   pros_.push_back(proto_ptr);
						   });

			regist_.regist("struct",
						   [&](std::fstream& ifs)
						   {
							   auto struct_ptr = std::make_shared<structure>();
							   struct_ptr->parse(ifs, column_, row_);
							   pros_.push_back(struct_ptr);
						   });

			regist_.regist("enum",
						   [&](std::fstream& ifs)
						   {
							   auto enum_ptr = std::make_shared<enum_struct>();
							   enum_ptr->parse(ifs, column_, row_);
							   pros_.push_back(enum_ptr);
						   });
		}

		bool parse::read_file(const std::string& file_name, const std::string& output)
		{
			std::fstream ifs(file_name, std::ios::in | std::ios::binary);

			if (!ifs.is_open())
			{
				std::cout << "file [" << file_name << "] not found\n";

				return false;
			}

			while (!ifs.eof())
			{
				auto result = read_value<token::key, ' '>(ifs, column_, row_)
								  .and_then(
									  [&](const auto& value) -> std::expected<std::string, parse_error>
									  {
										  auto f = regist_.invoke(value, column_, row_);

										  f(ifs);

										  return std::string{};
									  });

				if (result.has_value())
					continue;

				if (result.error() == parse_error::success)
					break;

				std::cout << result.error() << std::endl;

				return false;
			}

			std::filesystem::path out_path(file_name);

			std::filesystem::path final_output_path(output);
			final_output_path.append(out_path.filename().string());

			auto header_file = std::format("{}.h", final_output_path.string());
			auto src_file = std::format("{}.cpp", final_output_path.string());

			std::fstream ofs_h(header_file, std::ios::out);
			std::fstream ofs_s(src_file, std::ios::out);

			if (!ofs_h.is_open() || !ofs_s.is_open())
				return false;

			ofs_h << "#pragma once\n";

			for (auto& s : proto_types)
			{
				ofs_h << "#include <aquarius/virgo.hpp>\n\n";
			}

			for (auto& p : pros_)
			{
				p->generate(ofs_h);
			}

			ofs_h << "\n\n\n\n\n";

			for (auto& p : pros_)
			{
				p->generate_define(ofs_h);
			}

			ofs_s << "#include \"" << final_output_path.string() << "\"\n\n";

			for (auto& s : pros_)
			{
				s->generate_src(ofs_s);
			}

			return true;
		}

	} // namespace virgo
} // namespace aquarius
