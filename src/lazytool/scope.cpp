#include "scope.h"
#include "read_value.hpp"
#include "log.h"
#include "keyword_impl.h"

namespace aquarius
{
	namespace virgo
	{
		scope from_scope_string(const std::string& s)
		{
			if (s == "header")
			{
				return scope::s_header;
			}
			else if (s == "body")
			{
				return scope::s_body;
			}

			return scope::s_error;
		}

		std::expected<std::string, parse_error> router::parse(std::fstream& ifs, std::size_t column, std::size_t row)
		{
			return read_value<token::key, ':'>(ifs, column, row)
				.and_then(
					[&](const auto& key) -> std::expected<std::string, parse_error>
					{
						key_ = key;

						check_key(column, row);

						return read_value<token::path, '{'>(ifs, column, row)
							.and_then(
								[&](const auto& value) -> std::expected<std::string, parse_error>
								{
									value_ = value;

									return std::string{};
								});
					});
		}

		void router::generate(std::fstream& ofs)
		{
			ofs << "constexpr static auto id = " << value_ << ";\n";
		}

		std::string router::type() const
		{
			return type_;
		}
		std::string router::value() const
		{
			return value_;
		}

		void router::check_key(std::size_t column, std::size_t row)
		{
			if (key_ == "tcp-key")
			{
				type_ = "tcp";
			}
			else if (key_ == "udp-key")
			{
				type_ = "udp";
			}
			else if (key_ == "http-regex")
			{
				type_ = "http";
			}
			else if (key_ == "http-str")
			{
				type_ = "http";
			}
			else
			{
				throw std::runtime_error(
					log("check_value", std::format("not support router of {}!", type_), column, row));
			}
		}

		std::expected<std::string, parse_error> domain::read_domain(std::fstream& ifs, std::size_t column,
																	std::size_t row)
		{
			std::string value{};

			while (!ifs.eof())
			{
				auto c = ifs.get();

				if (std::iscntrl(c))
					continue;

				value.push_back(static_cast<char>(c));

				if (!std::isalpha(c))
					return value;
			}

			return std::unexpected(parse_error::domains);
		}

		std::expected<std::string, parse_error> domain::parse(std::fstream& ifs, std::size_t column, std::size_t row)
		{
			std::expected<std::string, parse_error> result;

			while (!ifs.eof())
			{
				result = read_domain(ifs, column, row)
							 .and_then(
								 [&](const auto& key) -> std::expected<std::string, parse_error>
								 {
									 auto ending = *key.rbegin();

									 if (ending == ' ')
									 {
										 scopes_.push_back({});
										 scopes_.back().first = key.substr(0, key.size() - 1);

										 if (!check_type(scopes_.back().first))
											 return std::unexpected(parse_error::type);

										 return read_value<token::value, ';'>(ifs, column, row)
											 .and_then(
												 [&](const auto& value) -> std::expected<std::string, parse_error>
												 {
													 scopes_.back().second = value;

													 return std::string{};
												 });
									 }
									 else if (ending == ':' || ending == '}')
									 {
										 int size = static_cast<int>(key.size());

										 ifs.seekg(-size, std::ios::cur);

										 return std::unexpected(parse_error::ending);
									 }

									 return std::unexpected(parse_error::syntax);
								 });

				if (!result.has_value())
				{
					break;
				}
			}

			return result;
		}

		void domain::generate(std::fstream& ofs)
		{
			for (auto& [type, value] : scopes_)
			{
				if (type.empty())
					continue;

				ofs << "\t" << from_type_string(type) << " " << value << ";\n";
			}
		}

		std::expected<std::string, parse_error> proto::parse(const std::string& parent, std::fstream& ifs,
															 std::size_t column, std::size_t row)
		{
			name_ = parent;

			std::expected<std::string, parse_error> result{};

			while (!ifs.eof())
			{
				auto res = seek<'}'>(ifs, column, row);

				if (res.has_value())
					return std::unexpected(parse_error::ending);

				result = read_value<token::key, ':'>(ifs, column, row)
							 .and_then(
								 [&](const auto& value) -> std::expected<std::string, parse_error>
								 {
									 auto tk = from_scope_string(value);

									 switch (tk)
									 {
									 case scope::s_header:
										 result = hr_.parse(ifs, column, row);
										 break;
									 case scope::s_body:
										 result = by_.parse(ifs, column, row);
										 break;
									 default:
										 return std::unexpected(parse_error::keyword);
									 }

									 return result;
								 });

				if (!result.has_value() && result.error() != parse_error::ending)
					break;
			}

			return result;
		}

		void proto::generate_header(std::fstream& ofs)
		{
			hr_.generate(ofs);
		}

		void proto::generate_body(std::fstream& ofs)
		{
			by_.generate(ofs);
		}

		void proto::generate_src_serialize(std::fstream& ofs, const std::string& type, const std::string& scope)
		{
			if (type.empty())
				return;

			if (scope == "header")
				return;

			if (type == "tcp")
			{
				ofs << "\t" << value << " = this->parse_" << scope << "_from" << "<" << name_+ "_" +scope << ">(buffer);\n";
			}
			else if (type == "http")
			{
				ofs << "\t *this = this->parse_" << scope << "_from" << "<" << name_ + "_" +scope << ">(buffer);\n";
			}
		}

		void proto::generate_src_deserialize(std::fstream& ofs, const std::string& type, const std::string& scope)
		{
			if (type.empty())
				return;

			if (scope == "header")
				return;

			if (type == "tcp")
			{
				ofs << "\t" << "this->parse_" << scope << "_to" << "(*this, buffer);\n";
			}
			else if (type == "http")
			{
				ofs << "\t" << "this->parse_" << scope << "_to" << "(*this, buffer);\n";
			}
		}

		void proto::generate_tag_invoke_define(std::fstream& ofs, const std::string& scope)
		{
			ofs << "\tfriend void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const "
				<< name_ << "_" << scope << "& local);\n\n";

			ofs << "\tfriend "<< name_ << "_" << scope <<" tag_invoke(const aquarius::json::value_to_tag<" << name_ << "_" << scope
				<< ">&, const aquarius::json::value& jv);\n";
		}

		void proto::generate_tag_invoke_src(std::fstream& ofs, const std::string& scope)
		{
			auto scopes = scope == "header" ? hr_.scopes_ : by_.scopes_;

			ofs << "void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const " << name_ << "_" << scope
				<< "& local)\n";
			ofs << "{\n";
			ofs << "\tauto& jv_obj = jv.emplace_object();\n";
			for (auto& [key, value] : scopes)
			{
				ofs << "\tjv_obj.emplace(\"" << value << "\", local." << value << "); \n";
			}
			ofs << "}\n\n";

			ofs << name_ << "_" << scope << " tag_invoke(const aquarius::json::value_to_tag<"
															<< name_ << "_" << scope << ">&, const aquarius::json::value& jv)\n";
			ofs << "{\n";
			ofs << "\t" << name_ << "_" << scope << " result{};\n";
			ofs << "\tauto obj = jv.try_as_object();\n";
			ofs << "\tif(obj->empty())\n";
			ofs << "\t\treturn {};\n\n";
			ofs << "\tauto param = obj->at(\"params\").as_object();\n";
			ofs << "\tif(param.empty())\n";
			ofs << "\t\treturn {};\n";
;
			for (auto& [key, value] : scopes)
			{
				ofs << "\tresult." << value <<" = static_cast<" << from_type_string(key) << ">(param.at(\"" << value << "\").as_" << from_json_type_string(key)
					<< "());\n";
			}
			ofs << "\treturn result;\n";

			ofs << "}\n\n";
		}

		std::string proto::name() const
		{
			return name_;
		}

		std::string proto::from_json_type_string(const std::string& type)
		{
			return type;
		}
	} // namespace virgo
} // namespace aquarius
