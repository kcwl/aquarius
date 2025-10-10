#include "keyword.h"
#include "read_value.hpp"
#include <filesystem>
#include <string_view>
#include "keyword_impl.h"

using namespace std::string_view_literals;

namespace
{
	constexpr std::string_view end = "};\n"sv;
}

namespace aquarius
{
	namespace virgo
	{
		std::expected<std::string, parse_error> protocol::parse(std::fstream& ifs, std::size_t& column,
																std::size_t& row, std::set<std::string>& proto_types)
		{
			return read_value<token::value, '-'>(ifs, column, row)
				.and_then(
					[&](const auto& value) -> std::expected<std::string, parse_error>
					{
						auto r = seek<'>'>(ifs, column, row);

						if (!r.has_value())
							return std::unexpected(parse_error::routers);

						name_ = value;

						auto res =
							router_.parse(ifs, column, row)
								.and_then(
									[&](const auto&) -> std::expected<std::string, parse_error>
									{
										proto_types.insert(router_.type());

										std::expected<std::string, parse_error> result;
										while (!ifs.eof())
										{
											auto res = seek<'}'>(ifs, column, row);
											if (res.has_value())
												return std::unexpected(parse_error::ending);

											result =
												read_value<token::key, '{'>(ifs, column, row)
													.and_then(
														[&](const auto& value)
															-> std::expected<std::string, parse_error>
														{
															std::expected<std::string, parse_error> rest{};

															if (value == "request")
																rest = request_.parse(name_ + "_req", ifs, column, row);
															else if (value == "response")
																rest =
																	response_.parse(name_ + "_resp", ifs, column, row);
															else
																return std::unexpected(parse_error::keyword);

															return rest;
														});

											if (!result.has_value() && result.error() != parse_error::ending)
											{
												break;
											}
										}

										return result;
									});

						if (!res.has_value() && res.error() != parse_error::ending)
							return std::unexpected(parse_error::syntax);

						return std::string{};
					});
		}


		void protocol::generate(std::fstream& ofs)
		{
			//ofs << "struct " << name_ << "_req_header\n";
			//ofs << "{\n";
			//request_.generate_header(ofs);
			//ofs << "};\n\n";
			//ofs << "struct " << name_ << "_req_body\n";
			//ofs << "{\n";
			//request_.generate_body(ofs);
			//ofs << "};\n\n";
			//ofs << "struct " << name_ << "_resp_header\n";
			//ofs << "{\n";
			//response_.generate_header(ofs);
			//ofs << "};\n\n";
			//ofs << "struct " << name_ << "_resp_body\n";
			//ofs << "{\n";
			//response_.generate_body(ofs);
			//ofs << "};\n\n";
			write_class(ofs, name_+"_req_header", this->router_.type());
			request_.generate_header(ofs);
			ofs << end << std::endl;

			write_class(ofs, name_+"_req_body", this->router_.type());
			if (this->router_.type() == "http")
			{
				request_.generate_tag_invoke_define(ofs, "body");
			}
			request_.generate_body(ofs);
			
			ofs << end << std::endl;

			write_class(ofs, name_+"_resp_header", this->router_.type());
			response_.generate_header(ofs);
			ofs << end << std::endl;

			write_class(ofs, name_+"_resp_body", this->router_.type());
			if (this->router_.type() == "http")
			{
				response_.generate_tag_invoke_define(ofs, "body");
			}
			response_.generate_body(ofs);
			ofs << end << std::endl;
		}

		void protocol::generate_define(std::fstream& ofs)
		{
			ofs << "using " << name_ << "_request = aquarius::virgo::" << router_.type() << "_request<" << "\"" << router_.value() << "\", " << name_ << "_req_header, " << name_ << "_req_body>;\n";
			ofs << "using " << name_ << "_response = aquarius::virgo::" << router_.type() << "_response<" << "\"" << router_.value() << "\", " << name_ << "_resp_header, " << name_ << "_resp_body>;\n";
		}

		void protocol::generate_src(std::fstream& ofs)
		{
			ofs << "bool operator==(const " << name_ << "_req_header& lhs, const " << name_ << "_req_header& rhs)\n";
			ofs << "{\n";
			request_.generate_equal(ofs, router_.type(), "header");
			ofs << "}\n\n";

			ofs << "std::ostream& operator<<(std::ostream& os, const " << name_ << "_req_header& other)\n";
			ofs << "{\n";
			request_.generate_stream(ofs, router_.type(), "header");
			ofs << "}\n\n";

			ofs << "void "<< name_ <<"_req_header::serialize(aquarius::detail::flex_buffer<char>& buffer)\n";
			ofs << "{\n";
			request_.generate_src_serialize(ofs, router_.type(), "header");
			ofs << "}\n\n";

			ofs << "void "<<name_<<"_req_header::deserialize(aquarius::detail::flex_buffer<char>& buffer)\n";
			ofs << "{\n";
			request_.generate_src_deserialize(ofs, router_.type(), "header");
			ofs << "}\n\n";

			ofs << "bool operator==(const " << name_ << "_req_body& lhs, const " << name_ << "_req_body& rhs)\n";
			ofs << "{\n";
			request_.generate_equal(ofs, router_.type(), "body");
			ofs << "}\n\n";

			ofs << "std::ostream& operator<<(std::ostream& os, const " << name_ << "_req_body& other)\n";
			ofs << "{\n";
			request_.generate_stream(ofs, router_.type(), "body");
			ofs << "}\n\n";

			ofs << "void "<< name_ <<"_req_body::serialize(aquarius::detail::flex_buffer<char>& buffer)\n";
			ofs << "{\n";
			request_.generate_src_serialize(ofs, router_.type(), "body");
			ofs << "}\n\n";

			ofs << "void "<<name_<<"_req_body::deserialize(aquarius::detail::flex_buffer<char>& buffer)\n";
			ofs << "{\n";
			request_.generate_src_deserialize(ofs, router_.type(), "body");
			ofs << "}\n\n";

			if (router_.type() == "http")
			{
				request_.generate_tag_invoke_src(ofs, "body");
			}

			ofs << "bool operator==(const " << name_ << "_resp_header& lhs, const " << name_ << "_resp_header& rhs)\n";
			ofs << "{\n";
			response_.generate_equal(ofs, router_.type(), "header");
			ofs << "}\n\n";

			ofs << "std::ostream& operator<<(std::ostream& os, const " << name_ << "_resp_header& other)\n";
			ofs << "{\n";
			response_.generate_stream(ofs, router_.type(), "header");
			ofs << "}\n\n";

			ofs << "void "<< name_ <<"_resp_header::serialize(aquarius::detail::flex_buffer<char>& buffer)\n";
			ofs << "{\n";
			response_.generate_src_serialize(ofs, router_.type(), "header");
			ofs << "}\n\n";

			ofs << "void "<<name_<<"_resp_header::deserialize(aquarius::detail::flex_buffer<char>& buffer)\n";
			ofs << "{\n";
			response_.generate_src_deserialize(ofs, router_.type(), "header");
			ofs << "}\n\n";

			ofs << "bool operator==(const " << name_ << "_resp_body& lhs, const " << name_ << "_resp_body& rhs)\n";
			ofs << "{\n";
			response_.generate_equal(ofs, router_.type(), "body");
			ofs << "}\n\n";

			ofs << "std::ostream& operator<<(std::ostream& os, const " << name_ << "_resp_body& other)\n";
			ofs << "{\n";
			response_.generate_stream(ofs, router_.type(), "body");
			ofs << "}\n\n";

			ofs << "void "<< name_ <<"_resp_body::serialize(aquarius::detail::flex_buffer<char>& buffer)\n";
			ofs << "{\n";
			response_.generate_src_serialize(ofs, router_.type(), "body");
			ofs << "}\n\n";

			ofs << "void "<<name_<<"_resp_body::deserialize(aquarius::detail::flex_buffer<char>& buffer)\n";
			ofs << "{\n";
			response_.generate_src_deserialize(ofs, router_.type(), "body");
			ofs << "}\n\n";

			if (router_.type() == "http")
			{
				response_.generate_tag_invoke_src(ofs, "body");
			}
		}

		void protocol::write_class(std::fstream& ofs, const std::string& name, const std::string& type)
		{
			ofs << "class " << name <<  " final : public aquarius::virgo::" << type << "_serialize\n";
			ofs << "{\n";
			ofs << "public:\n";
			ofs << "\t" << name << "() = default; \n";
			ofs << "\tvirtual ~"<< name <<"() = default; \n\n";
			ofs << "public:\n";
			ofs << "\tvirtual void serialize(aquarius::detail::flex_buffer<char>& buffer) override; \n\n";
			ofs << "\tvirtual void deserialize(aquarius::detail::flex_buffer<char>& buffer) override; \n";
		}

		std::expected<std::string, parse_error> structure::parse(std::fstream& ifs, std::size_t column, std::size_t row)
		{
			return read_value<token::value, '{'>(ifs, column, row)
				.and_then(
					[&](const auto& value) -> std::expected<std::string, parse_error>
					{
						name_ = value;

						put_custom_type(name_);

						std::expected<std::string, parse_error> result;

						while (!ifs.eof())
						{
							auto res = seek<'}'>(ifs, column, row);

							if (res.has_value())
								return std::unexpected(parse_error::ending);

							result =
								read_value<token::key, ' '>(ifs, column, row)
									.and_then(
										[&](const auto& key)
										{
											scopes_.push_back({});
											auto& content = scopes_.back();

											content.first = key;

											return read_value<token::value, ';'>(ifs, column, row)
												.and_then(
													[&](const auto& value) -> std::expected<std::string, parse_error>
													{
														content.second = value;

														return std::string{};
													});
										});

							if (!result.has_value())
								break;
						}

						return result;
					});
		}

		void structure::generate(std::fstream& ofs_h)
		{
			ofs_h << "struct " << name_ << "\n";
			ofs_h << "{\n";
			for (auto& [type, name] : scopes_)
			{
				ofs_h << "    " << from_type_string(type) << " " << name << ";\n";
			}
			ofs_h << "\n";
			ofs_h << "};\n";

			ofs_h << "inline bool operator==(const " << name_ << "& lhs, const " << name_ << "& rhs)\n";
			ofs_h << "\t{\n";
			ofs_h << "\t\treturn ";
			for (auto& [_, name] : scopes_)
			{
				ofs_h << "lhs." << name << " == rhs." << name << " && ";
			}

			ofs_h.seekg(-4, std::ios::cur);
			ofs_h << ";\n";
			ofs_h << "\t}\n\n";

			ofs_h << "\tinline std::ostream& operator<<(std::ostream& os, const " << name_ << "& other)\n";
			ofs_h << "\t{\n";
			ofs_h << "\t\tos << ";

			for (auto& [type, name] : scopes_)
			{
				if (type.contains("bytes"))
				{
					ofs_h << "\"[\";\n";
					ofs_h << "\t\tfor (auto& s : other." << name << ")\n";
					ofs_h << "\t\t{\n";
					ofs_h << "\t\t\tos << s << \", \";\n";
					ofs_h << "\t\t}\n";

					ofs_h << "\t\tos.seekp(-2, std::ios::cur);\n";
					ofs_h << "\t\tos << \"]\";\n";
				}
				else
				{
					ofs_h << "other." << name << " << ";
				}
			}

			ofs_h << "\t\treturn os;\n";

			ofs_h << "\t}\n";
		}

		std::expected<std::string, parse_error> enum_struct::parse(std::fstream& ifs, std::size_t column,
																   std::size_t row)
		{
			return read_value<token::value, '{'>(ifs, column, row)
				.and_then(
					[&](const auto& value) -> std::expected<std::string, parse_error>
					{
						name_ = value;

						put_custom_type(name_);

						std::expected<std::string, parse_error> result;

						while (!ifs.eof())
						{
							auto res = seek<'}'>(ifs, column, row);

							if (res.has_value())
								return std::unexpected(parse_error::ending);

							result = read_value<token::value, ' '>(ifs, column, row)
										 .and_then(
											 [&](const auto& value) -> std::expected<std::string, parse_error>
											 {
												 scopes_.push_back(value);

												 return std::string{};
											 });

							if (!result.has_value())
								break;
						}

						return result;
					});
		}

		void enum_struct::generate(std::fstream& ofs_h, std::fstream& /*ofs_s*/)
		{
			ofs_h << "enum class " << name_ << "\n";
			ofs_h << "{\n";
			for (auto& name : scopes_)
			{
				ofs_h << "    " << name << ",\n";
			}
			ofs_h << "};\n";
		}
	} // namespace virgo
} // namespace aquarius
