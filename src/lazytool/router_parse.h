#pragma once
#include "parser.h"
#include <ostream>

namespace aquarius
{
	namespace lazytool
	{
		class router_parse : public parser
		{
		public:
			router_parse() = default;
			virtual ~router_parse() = default;

		public:
			std::ostream& operator<<(std::ostream& os) const;

		public:
			virtual parse_error visit(std::ifstream& ifs, std::size_t& column, std::size_t& row) override;

		private:
			bool check_protocol_type(const std::string& protocol_type);

			bool check_mode(const std::string& mode);

		public:
			std::string value_;

			std::string mode_;
		};

		std::ostream& operator<<(std::ostream& os, const aquarius::lazytool::router_parse& r);
	} // namespace lazytool
} // namespace aquarius