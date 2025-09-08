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
			return read_value<token::key, ':'>(ifs, column, row)
				.and_then(
					[&] (const auto& key) ->std::expected<std::string, parse_error>
					{
						key_ = key;

						check_key(column, row);

						return read_value<token::path, '{'>(ifs, column, row)
							.and_then(
								[&] (const auto& value) ->std::expected<std::string, parse_error>
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


		std::expected<std::string, parse_error> domain::read_domain(std::fstream& ifs, std::size_t column, std::size_t row)
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
						[&] (const auto& key) -> std::expected<std::string, parse_error>
						{
							auto ending = *key.rbegin();

							if (ending == ' ')
							{
								scopes_.push_back({});
								scopes_.back().first = key.substr(0, key.size());

								return read_value<token::value, ';'>(ifs, column, row)
									.and_then(
										[&] (const auto& value) -> std::expected<std::string, parse_error>
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

		void domain::generate(const std::string& name, std::fstream& ofs)
		{
			ofs << "struct " << name << "\n";
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

				result = read_value<token::key, ':'>(ifs, column, row)
					.and_then(
						[&] (const auto& value) -> std::expected<std::string, parse_error>
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

		void proto::generate(const std::string& name, std::fstream& ofs)
		{
			ofs << "struct " << name << "::" << name_ << "\n";
			ofs << "{\n";
			hr_.generate("header",ofs);
			by_.generate("body", ofs);
			ofs << "};\n";
		}

		std::string proto::name() const
		{
			return name_;
		}
	} // namespace virgo
} // namespace aquarius
