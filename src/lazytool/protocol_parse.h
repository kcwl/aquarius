#pragma once
#include "parser.h"

namespace aquarius
{
	namespace lazytool
	{
		class domin_parse;

		class protocol_parse : public parser
		{
			enum class scope
			{
				s_header,
				s_body,
				s_error
			};

		public:
			protocol_parse();
			virtual ~protocol_parse() = default;

		public:
			std::ostream& operator<<(std::ostream& os) const;

		public:
			virtual parse_error visit(std::ifstream& ifs, std::size_t& column, std::size_t& row) override;

		private:
			scope from_scope_string(const std::string& s);

		public:
			std::shared_ptr<domin_parse> header_ptr_;

			std::shared_ptr<domin_parse> body_ptr_;
		};

		std::ostream& operator<<(std::ostream& os, const protocol_parse& pp);
	} // namespace lazytool
} // namespace aquarius