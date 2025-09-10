#pragma once
#include "scope.h"
#include <string>
#include <expected>
#include "error.hpp"
#include <set>

namespace aquarius
{
	namespace virgo
	{
		class proto_keyword
		{
		public:
			virtual void generate(std::fstream& /*ofs_h*/)
			{}

			virtual void generate_define(std::fstream& /*ofs_h*/)
			{}
		};

		class protocol : public proto_keyword
		{
		public:
			std::expected<std::string, parse_error> parse(std::fstream& ifs, std::size_t& column, std::size_t& row, std::set<std::string>& proto_types);

			void generate(std::fstream& ofs);

			void generate_define(std::fstream& ofs);

		public:
			std::string name_;

			proto request_;

			proto response_;

			router router_;
		};

		class structure : public proto_keyword
		{
		public:
			structure() = default;

		public:
			std::expected<std::string, parse_error> parse(std::fstream& ifs, std::size_t column, std::size_t row);

			void generate(std::fstream& ofs_h, std::fstream& ofs_s);

		private:
			std::string name_;

			std::vector<std::pair<std::string, std::string>> scopes_;
		};

		class enum_struct : public proto_keyword
		{
		public:
			enum_struct() = default;

		public:
			std::expected<std::string, parse_error> parse(std::fstream& ifs, std::size_t column, std::size_t row);

			void generate(std::fstream& ofs_h, std::fstream& ofs_s);

		private:
			std::string name_;

			std::vector<std::string> scopes_;
		};
	} // namespace virgo
} // namespace aquarius
