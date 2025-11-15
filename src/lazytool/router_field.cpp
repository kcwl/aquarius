#include "router_field.h"
#include <set>
#include <map>

namespace
{
	static const std::set<std::string> router_protocol = { "tcp", "http" };

	static const std::map<std::string, std::vector<std::string>> router_mode =
	{
		{"tcp", {"binary"}},
		{"http", {"kv", "json", "html"}}
	};
}

namespace aquarius
{
	namespace lazytool
	{

        router_field::router_field()
            : parser(struct_type::router)
        {

        }

        parser::parse_error router_field::visit(std::ifstream& ifs, std::size_t& column, std::size_t& row)
        {
            auto value = read_value<token::key, '-'>(ifs, column, row);
            if (!check_protocol_type(value))
                return parse_error::routers;
            this->set_protocol(value);
            value = read_value<token::key, ':'>(ifs, column, row);
            if (!check_mode(this->protocol(), value))
                return parse_error::routers_mode;
            this->set_method(value);
            value = read_value<token::path, '{'>(ifs, column, row);
            if (value.empty())
                return parse_error::routers_value;
            this->set_key(value);
            return parse_error::success;
        }

        void router_field::set_protocol(const std::string& protocol)
        {
            protocol_ = protocol;
        }

        std::string router_field::protocol() const
        {
            return protocol_;
        }

        void router_field::set_method(const std::string& method)
        {
            method_ = method;
        }

        std::string router_field::method() const
        {
            return method_;
        }

        void router_field::set_key(const std::string& key)
        {
            key_ = key;
        }

        std::string router_field::key() const
        {
            return key_;
        }

        bool router_field::check_protocol_type(const std::string& protocol_type)
        {
            return router_protocol.find(protocol_type) != router_protocol.end();
        }

        bool router_field::check_mode(const std::string& protocol, const std::string& mode)
        {
            auto iter = router_mode.find(protocol);
            if (iter == router_mode.end())
            {
                return false;
            }
            auto& modes = iter->second;
            return std::find(modes.begin(), modes.end(), mode) != modes.end();
        }
	} // namespace lazytool
} // namespace aquarius
