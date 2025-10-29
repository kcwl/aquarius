#pragma once
#include "parser.h"
#include "data_field.h"

namespace aquarius
{
	namespace lazytool
	{
        class router_field;
        class service_struct;

        class protocol_struct : public data_field, public parser
        {

        public:
            protocol_struct();
            ~protocol_struct() = default;

        public:
            friend std::ostream& operator<<(std::ostream& os, const protocol_struct& field);

        public:
            virtual parse_error visit(std::ifstream& ifs, std::size_t& column, std::size_t& row) override;

            std::shared_ptr<service_struct> request() const;

            std::shared_ptr<service_struct> response() const;

            std::string protocol() const;

            std::string method() const;

            std::string router_key() const;

        private:
            std::shared_ptr<router_field> router_ptr_;
            std::shared_ptr<service_struct> request_ptr_;
            std::shared_ptr<service_struct> response_ptr_;
        };

	} // namespace lazytool
} // namespace aquarius