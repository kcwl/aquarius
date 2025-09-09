#include "keyword.h"
#include "read_value.hpp"
#include <filesystem>
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

		void protocol::generate(std::fstream& ofs_h, std::fstream& ofs_s)
		{
			ofs_h << "namespace " << name_ << "{\n";
			ofs_h << "struct " << name_ << "_req\n";
			ofs_h << "{\n";
			ofs_h << "    struct header;\n";
			ofs_h << "    struct body;\n";
			ofs_h << "};\n\n";
			ofs_h << "struct " << name_ << "_resp\n";
			ofs_h << "{\n";
			ofs_h << "    struct header;\n";
			ofs_h << "    struct body;\n";
			ofs_h << "};\n\n";
			ofs_h << "static constexpr std::string_view router() { return \"" << router_.value() << "\"sv; }\n";
			ofs_h << "using request = aquarius::virgo::" << router_.type() << "_request<" << name_ << "_req::header, " << name_ << "_req::body>;\n";
			ofs_h << "using response = aquarius::virgo::" << router_.type() << "_response<" << name_ << "_resp::header, " << name_ << "_resp::body>;\n";
			ofs_h << "}\n";

			ofs_s << "namespace " << name_ << "{\n";
			request_.generate(name_+"_req", ofs_s);
			response_.generate(name_+"_resp", ofs_s);
			ofs_s << "}\n";
		}

		std::expected<std::string, parse_error> structure::parse(std::fstream& ifs, std::size_t column, std::size_t row)
		{
			return read_value<token::value, '{'>(ifs, column, row)
				.and_then(
					[&](const auto& value) -> std::expected<std::string, parse_error>
					{
						name_ = value;

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

		void structure::generate(std::fstream& ofs_h, std::fstream& /*ofs_s*/)
		{
			ofs_h << "struct " << name_ << "\n";
			ofs_h << "{\n";
			for (auto& [type, name] : scopes_)
			{
				ofs_h << "    " << type << " " << name << ";\n";
			}
			ofs_h << "};\n";
		}

		std::expected<std::string, parse_error> enum_struct::parse(std::fstream& ifs, std::size_t column,
																   std::size_t row)
		{
			return read_value<token::value, '{'>(ifs, column, row)
				.and_then(
					[&](const auto& value) -> std::expected<std::string, parse_error>
					{
						name_ = value;

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
