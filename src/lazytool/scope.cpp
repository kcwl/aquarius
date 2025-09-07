#include "scope.h"
#include "read_value.hpp"
#include "log.h"
#include "keyword.h"

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
			if (ifs.eof())
				return std::unexpected(parse_error::eof);

			return read_value<' '>(ifs, column, row, token::key)
				.and_then(
					[&](const auto& key) ->std::expected<std::string, parse_error>
					{
						key_ = key;

						check_key(column, row);

						return read_value<';'>(ifs, column, row, token::path)
							.and_then(
								[&](const auto& value) ->std::expected<std::string, parse_error>
								{
									value_ = value;

									auto res = seek<'}'>(ifs, column, row);

									if (res.has_value())
										return std::unexpected(parse_error::ending);

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

		std::expected<std::string, parse_error> header::parse(std::fstream& ifs, std::size_t column, std::size_t row)
		{
			if (ifs.eof())
				return std::unexpected(parse_error::eof);

			std::expected<std::string, parse_error> result;

			while (!ifs.eof())
			{
				auto res = seek<'}'>(ifs, column, row);

				if (res.has_value())
					return std::unexpected(parse_error::ending);

				result = read_value<' '>(ifs, column, row)
							 .and_then(
								 [&](const auto& key) -> std::expected<std::string, parse_error>
								 {
									 scopes_.push_back({});
									 auto& content = scopes_.back();
									 content.first = key;

									 // if (!check_keyword_type(content.first))
									 //  return std::unexpected(std::format("not support keyword of {}!",
									 //  content.first));

									 return read_value<';'>(ifs, column, row)
										 .and_then(
											 [&](const auto& value) -> std::expected<std::string, parse_error>
											 {
												 content.second = value;

												 return std::string{};
											 });
								 });

				if (!result.has_value())
				{
					break;
				}
			}

			return result;
		}

		void header::generate(std::fstream& ofs)
		{
			ofs << "struct header\n";
			ofs << "{\n";
			for (auto& [type, value] : scopes_)
			{
				if (type.empty())
					continue;

				ofs << "  " << type << " " << value << ";\n";
			}
			ofs << "};\n";
		}

		std::expected<std::string, parse_error> body::parse(std::fstream& ifs, std::size_t column, std::size_t row)
		{
			if (ifs.eof())
				return std::unexpected(parse_error::eof);

			std::expected<std::string, parse_error> result;

			while (!ifs.eof())
			{
				auto res = seek<'}'>(ifs, column, row);

				if (res.has_value())
					return std::unexpected(parse_error::ending);

				result = read_value<' '>(ifs, column, row)
							 .and_then(
								 [&](const auto& key) -> std::expected<std::string, parse_error>
								 {
									 scopes_.push_back({});
									 auto& content = scopes_.back();
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
		}

		void body::generate(std::fstream& ofs)
		{
			ofs << "struct body\n";
			ofs << "{\n";
			for (auto& [type, value] : scopes_)
			{
				if (type.empty())
					continue;

				ofs << "  " << type << " " << value << ";\n";
			}
			ofs << "};\n";
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

				result = read_value<':'>(ifs, column, row, token::key)
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

				if (!result.has_value())
				{
					if (result.error() == parse_error::ending)
						continue;

					break;
				}
			}

			return result;
		}

		void proto::generate(const std::string& name, std::fstream& ofs)
		{
			ofs << "struct " << name << "::" << name_ << "\n";
			ofs << "{\n";
			hr_.generate(ofs);
			by_.generate(ofs);
			ofs << "};\n";
		}

		std::string proto::name() const
		{
			return name_;
		}
	} // namespace virgo
} // namespace aquarius
