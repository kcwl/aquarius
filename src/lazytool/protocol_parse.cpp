#include "protocol_parse.h"
#include "domin_parse.h"
#include "router_field.h"
#include "service_struct.h"

namespace aquarius
{
	namespace lazytool
	{
		protocol_struct::protocol_struct()
			: parser(struct_type::protocol)
			, router_ptr_(new router_field())
			, request_ptr_(new service_struct())
			, response_ptr_(new service_struct())
		{}

		std::ostream& operator<<(std::ostream& os, const protocol_struct& field)
		{
			os << "class " << field.name() << "_protocol" << std::endl;
			os << "{" << std::endl;

			os << "\t" << "struct request_service" << std::endl;
			os << "\t{" << std::endl;

			os << "\t\t" << "struct header_domin" << std::endl;
			for (auto& s : field.request_ptr_->header()->fields())
			{
				os << "\t\t" << s.first << " " << s.second << ";" << std::endl;
			}
			os << "\t\t" << "struct body_domin" << std::endl;
			for (auto& s : field.request_ptr_->body()->fields())
			{
				os << "\t\t" << s.first << " " << s.second << ";" << std::endl;
			}

			os << "\t};" << std::endl;

			os << "\t" << "struct response_service" << std::endl;
			os << "\t{" << std::endl;
			os << "\t\t" << "struct header_domin" << std::endl;
			for (auto& s : field.request_ptr_->header()->fields())
			{
				os << "\t\t" << s.first << " " << s.second << ";" << std::endl;
			}
			os << "\t\t" << "struct body_domin" << std::endl;
			for (auto& s : field.request_ptr_->body()->fields())
			{
				os << "\t\t" << s.first << " " << s.second << ";" << std::endl;
			}
			os << "\t};" << std::endl;

			os << "};" << std::endl;

			return os;
		}

		protocol_struct::parse_error protocol_struct::visit(std::ifstream& ifs, std::size_t& column, std::size_t& row)
		{
			auto value = read_value<token::value, '-'>(ifs, column, row);

			if (value.empty())
				return parse_error::syntax;

			this->set_name(value);

			auto result = seek<'>'>(ifs, column, row);
			if (result != parse_error::success)
				return result;
			result = router_ptr_->visit(ifs, column, row);
			if (result != parse_error::success)
				return result;

			while (!ifs.eof())
			{
				result = seek<'}'>(ifs, column, row);
				if (result == parse_error::success)
					return result;
				auto sub_key = read_value<token::key, '{'>(ifs, column, row);
				if (sub_key == "request")
				{
					result = request_ptr_->visit(ifs, column, row);
					request_ptr_->set_name(this->name());
				}
				else if (sub_key == "response")
				{
					result = response_ptr_->visit(ifs, column, row);
					response_ptr_->set_name(this->name());
				}
				else
					return parse_error::keyword;
			}
			return parse_error::eof;
		}

		std::shared_ptr<service_struct> protocol_struct::request() const
		{
			return request_ptr_;
		}

		std::shared_ptr<service_struct> protocol_struct::response() const
		{
			return response_ptr_;
		}

		std::string protocol_struct::protocol() const
		{
			return router_ptr_->protocol();
		}

		std::string protocol_struct::method() const
		{
			return router_ptr_->method();
		}

		std::string protocol_struct::router_key() const
		{
			return router_ptr_->key();
		}
	} // namespace lazytool
} // namespace aquarius