#pragma once
#include "parser.h"

namespace aquarius
{
	namespace lazytool
	{
		class protocol_parse;
		class router_parse;

		class message_parse : public parser
		{
		public:
			message_parse();
			virtual ~message_parse() = default;

		public:
			virtual parse_error visit(std::ifstream& ifs, std::size_t& column, std::size_t& row) override;

		public:
			std::shared_ptr<router_parse> router_ptr_;

			std::shared_ptr<protocol_parse> request_ptr_;

			std::shared_ptr<protocol_parse> response_ptr_;
		};
	} // namespace lazytool
} // namespace aquarius