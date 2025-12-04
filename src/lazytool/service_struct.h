#pragma once
#include "data_field.h"
#include "parser.h"

namespace aquarius
{
	namespace lazytool
	{
		class domin_struct;

		class service_struct : public data_field, public parser
		{
		public:
			service_struct();
			~service_struct() = default;

		public:
			virtual parse_error visit(std::ifstream& ifs, std::size_t& column, std::size_t& row) override;

			std::shared_ptr<domin_struct> header() const;

			std::shared_ptr<domin_struct> body() const;

		private:
			std::shared_ptr<domin_struct> header_ptr_;

			std::shared_ptr<domin_struct> body_ptr_;
		};
	} // namespace lazytool
} // namespace aquarius