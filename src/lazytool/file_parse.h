#pragma once
#include "registor.h"
#include <memory>
#include <vector>
#include "struct_type.hpp"

namespace aquarius
{
	namespace lazytool
	{
        class data_field
        {
        public:
            data_field(struct_type s = struct_type::structure);

            ~data_field() = default;

        public:
            bool add_field(const std::string& type, const std::string& value);

            std::string name() const;

            struct_type type() const;

            void set_name(const std::string& name);

            std::vector<std::pair<std::string, std::string>> fields() const;

        private:
            std::string name_;

            struct_type type_;

            std::vector<std::pair<std::string, std::string>> fields_;
        };

        std::ostream& operator<<(std::ostream& os, const data_field& field);

        class router_field
        {
        public:
            router_field() = default;
            ~router_field() = default;

        public:
            bool set_protocol(const std::string& protocol);

            std::string protocol() const;

            bool set_method(const std::string& method);

            std::string method() const;

            void set_router(const std::string& router);

            std::string router() const;

        private:
            bool check_protocol_type(const std::string& protocol_type);

            bool check_mode(const std::string& protocol, const std::string& mode);

        private:
            std::string protocol_;

            std::string method_;

            std::string router_;
        };

        class message_field : public router_field, public data_field
        {
        public:
            message_field();

            ~message_field() = default;

        public:
            void set_request(std::shared_ptr<data_field> request);

            std::shared_ptr<data_field> request();

            void set_response(std::shared_ptr<data_field> response);

            std::shared_ptr<data_field> response();

        private:
            std::shared_ptr<data_field> request_ptr_;

            std::shared_ptr<data_field> response_;
        };

        class file_parser
        {
        public:
            file_parser();
            ~file_parser() = default;

        public:
            bool read_file(const std::string& file_path);

        public:
            std::vector<std::shared_ptr<data_field>> fields_;

            registor registor_;

            std::size_t column_;

            std::size_t row_;
        };
	} // namespace lazytool
} // namespace aquarius
