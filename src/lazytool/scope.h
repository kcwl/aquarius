#pragma once
#include <fstream>
#include <vector>
#include <expected>
#include "error.hpp"

namespace aquarius
{
	namespace virgo
	{
		enum class scope
		{
			s_header,
			s_body,
			s_error
		};

		scope from_scope_string(const std::string& s);

		class header
		{
		public:
			std::expected<std::string, parse_error> parse(std::fstream& ifs, std::size_t column, std::size_t row);

			void generate(std::fstream& ofs);

		public:
			std::vector<std::pair<std::string, std::string>> scopes_;
		};

		class body
		{
		public:
			std::expected<std::string, parse_error> parse(std::fstream& ifs, std::size_t column, std::size_t row);

			void generate(std::fstream& ofs);

		public:
			std::vector<std::pair<std::string, std::string>> scopes_;
		};

		class router
		{
		public:
			std::expected<std::string, parse_error> parse(std::fstream& ifs, std::size_t column, std::size_t row);

			void generate(std::fstream& ofs);

			std::string type() const;

		private:
			void check_key(std::size_t column, std::size_t row);

		public:
			std::string key_;

			std::string value_;

			std::string type_;
		};

		class proto
		{
		public:
			proto() = default;

		public:
			std::expected<std::string, parse_error> parse(const std::string& parent, std::fstream& ifs,
														  std::size_t column, std::size_t row);

			void generate(const std::string& name, std::fstream& ofs);

			std::string name() const;

		private:
			std::string name_;

			header hr_;

			body by_;
		};
	} // namespace virgo
} // namespace aquarius
