#pragma once
#include "parser.h"
#include "data_field.h"

namespace aquarius
{
	namespace lazytool
	{

        class structure : public data_field, public parser
        {
        public:
            structure()
                : parser(struct_type::structure)
            {

            }
            ~structure() = default;
        public:
            friend std::ostream& operator<<(std::ostream& os, const structure& field)
            {
                os << "struct " << field.name() << std::endl;
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
        };
	} // namespace lazytool
} // namespace aquarius