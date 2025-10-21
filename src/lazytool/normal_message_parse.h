#pragma once
#include "parser.h"


namespace aquarius
{
	namespace lazytool
	{

		class router_parse;

		class structure_parse;

		class normal_message_parse : public parser
		{
		public:
			normal_message_parse();
			virtual ~normal_message_parse() = default;

		public:
			std::ostream& operator<<(std::ostream& os) const;

		public:
			virtual parse_error visit(std::ifstream& ifs, std::size_t& column, std::size_t& row) override;

		public:
			std::shared_ptr<router_parse> router_ptr_;

			std::shared_ptr<structure_parse> request_ptr_;

			std::shared_ptr<structure_parse> response_ptr_;
		};
	} // namespace lazytool
} // namespace aquarius