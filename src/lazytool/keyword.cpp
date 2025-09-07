#include "keyword.h"
#include "read_value.hpp"
#include <filesystem>
namespace aquarius
{
	namespace virgo
	{
		std::expected<std::string, parse_error> protocol::parse(std::fstream& ifs, std::size_t& column,
																std::size_t& row)
		{
			return read_value<'{'>(ifs, column, row)
				.and_then(
					[&](const auto& value) -> std::expected<std::string, parse_error>
					{
						name_ = value;

						while (!ifs.eof())
						{
							auto result = read_value<'{'>(ifs, column, row, token::key)
											  .and_then(
												  [&](const auto& value) -> std::expected<std::string, parse_error>
												  {
													  std::expected<std::string, parse_error> rest{};

													  if (value == "request")
														  rest = request_.parse(name_ + "_req", ifs, column, row);
													  else if (value == "response")
														  rest = response_.parse(name_ + "_resp", ifs, column, row);
													  else if (value == "router")
														  rest = router_.parse(ifs, column, row);
													  else
														  return std::unexpected(parse_error::keyword);

													  return rest;
												  });

							if (!result.has_value())
							{
								if (result.error() == parse_error::ending)
									continue;

								return result;
							}
						}

						auto res = seek<'}'>(ifs, column, row);
						if (!res.has_value())
							return std::unexpected(parse_error::syntax);

						return std::string{};
					});
		}

		void protocol::generate(std::fstream& ofs_h, std::fstream& ofs_s)
		{
			ofs_h << "struct " << name_ << "_protocol\n";
			ofs_h << "{\n";
			ofs_h << "private:\n";
			ofs_h << "    struct " << request_.name() << ";\n";
			ofs_h << "    struct " << response_.name() << ";\n";
			ofs_h << "public:\n";
			ofs_h << "    using request = virgo::" << router_.type() << "::request<" << request_.name() << ">;\n";
			ofs_h << "    using response = virgo::" << router_.type() << "::request<" << request_.name() << ">;\n";
			ofs_h << "};\n";

			request_.generate(name_, ofs_s);

			response_.generate(name_, ofs_s);
		}

		std::expected<std::string, parse_error> structure::parse(std::fstream& ifs, std::size_t column, std::size_t row)
		{
			return read_value<'{'>(ifs, column, row)
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

							scopes_.push_back({});
							auto& content = scopes_.back();

							result = read_value<' '>(ifs, column, row)
										 .and_then(
											 [&](const auto& key)
											 {
												 content.first = key;

												 return read_value<';'>(ifs, column, row)
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
			return read_value<'{'>(ifs, column, row)
				.and_then(
					[&](const auto& value) -> std::expected<std::string, parse_error>
					{
						name_ = value;

						std::expected<std::string, parse_error> result;

						while (!ifs.eof())
						{
							auto res = seek<'}'>(ifs, column, row);

							if (res.has_value())
								break;

							result = read_value<' '>(ifs, column, row)
										 .and_then(
											 [&](const auto& value) -> std::expected<std::string, parse_error>
											 {
												 scopes_.push_back({});
												 auto& content = scopes_.back();

												 content = value;

												 return content;
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
