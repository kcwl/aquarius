#pragma once
#include "parser.h"
#include "data_field.h"

namespace aquarius
{
	namespace lazytool
	{
        class router_field;

        class message_struct : public data_field, public parser
        {
        public:
            message_struct();
            ~message_struct() = default;

            friend std::ostream& operator<<(std::ostream& os, const message_struct& field)
            {
                os << "class " << field.name() << std::endl;
                os << "{" << std::endl;

                for (auto& s : field.fields())
                {
                    os << "\t" << s.first << " " << s.second << ";" << std::endl;
                }

                os << "};" << std::endl;

                return os;
            }

        public:
            virtual parse_error visit(std::ifstream& ifs, std::size_t& column, std::size_t& row) override;

            std::string protocol() const;

            std::string method() const;

            void method(const std::string& m);

            std::string router_key() const;

        private:
            std::shared_ptr<router_field> router_ptr_;
        };
	} // namespace lazytool
} // namespace aquarius