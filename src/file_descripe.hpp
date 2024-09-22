#include <fstream>
#include <iostream>
#include <string>
#include <functional>
#include <map>
#include "command_line.hpp"
#include "message_info.hpp"



class file_descriper
{
public:
	file_descriper()
		: file_descriper(command_line{})
	{

	}

	file_descriper(const command_line& line)
		: file_name_(line.input_file_name_)
		, file_content_()
		, infos_()
	{
		struct_invokes_.emplace("syntax", [&](message_info& info, const std::string& keyword) {return this->parse_syntax(info, keyword); });
		struct_invokes_.emplace("import", [&](message_info& info, const std::string& keyword) {return this->parse_header(info, keyword); });
		struct_invokes_.emplace("namespace", [&](message_info& info, const std::string& keyword) {return this->parse_header(info, keyword); });
		struct_invokes_.emplace("message", [&](message_info& info, const std::string& keyword) {return this->parse_message(info, keyword); });
		struct_invokes_.emplace("enum", [&](message_info& info, const std::string& keyword) {return this->parse_enum(info, keyword); });
		struct_invokes_.emplace("[[request]]", [&](message_info& info, const std::string& keyword) 
			{
				info.special_key = "request";

				return this->parse_message(info, {});
			});
		struct_invokes_.emplace("[[response]]", [&](message_info& info, const std::string& keyword)
			{
				info.special_key = "response";

				return this->parse_message(info, {});
			});
	}

public:
	bool load()
	{
		file_content_.open(file_name_ + "." + suffix_str.data(), std::ios::in | std::ios::binary);

		if (!file_content_.is_open())
			return false;

		return true;
	}

	bool parse()
	{
		while (!file_content_.eof())
		{
			auto type = get_content_type();

			infos_.push_back({});
			auto& info = infos_.back();

			if (!parse_type(info, type))
				return false;
		}

		return true;
	}

	std::vector<message_info> get_message_infos() const
	{
		return infos_;
	}

	bool exist(const std::string& type)
	{
		auto iter_key = std::find(message_key.begin(), message_key.end(), type);

		auto iter_pod = std::find(pod_type_array.begin(), pod_type_array.end(), type);

		auto iter_complex = std::find(complex_type_array.begin(), complex_type_array.end(), type);

		auto iter_custom = std::find(custom_types_.begin(), custom_types_.end(), type);

		return iter_key != message_key.end() || iter_pod != pod_type_array.end() || iter_complex != complex_type_array.end() || iter_custom != custom_types_.end();
	}

	std::string syntax() const
	{
		return syntax_;
	}


private:
	bool parse_type(message_info& info, const std::string& type)
	{
		auto iter = struct_invokes_.find(type);

		if (iter == struct_invokes_.end())
		{
			std::cout << "maybe some keyword not exist! type:" << type << std::endl;
			return false;
		}

		if (!iter->second(info, type))
			return false;

		return true;
	}

	std::string get_content_type()
	{
		return trace_key();
	}

	bool parse_syntax(message_info& info, const std::string& key_world = {})
	{
		if (!parse_header(info, key_world))
		{
			std::cout << "message is not incomplete\n";

			return false;
		}

		syntax_ = info.type_name;

		return true;
	}

	bool parse_message(message_info& info, const std::string& key_world = {})
	{
		if (!parse_header(info, key_world))
		{
			std::cout << "message is not incomplete\n";

			return false;
		}

		if (!info.special_key.empty())
		{
			if (!parse_special_number(info))
			{
				std::cout << "special number is invalid\n";

				return false;
			}
		}

		if (!parse_start())
		{
			std::cout << "message not has start flag\n";

			return false;
		}

		if (!parse_body(info))
		{
			std::cout << "message body format error\n";

			return false;
		}

		if (!parse_end())
		{
			std::cout << "message not has end flag\n";

			return false;
		}

		return true;
	}

	bool parse_special_key(message_info& info, bool& has_special_key)
	{
		if (file_content_.eof())
			return false;

		auto cur = file_content_.peek();

		if (cur != '[')
			return true;

		file_content_.get();

		cur = file_content_.get();

		if (cur != '[')
			return false;

		auto result = trace_key(']');

		cur = file_content_.peek();

		if (cur != ']')
			return false;

		file_content_.get();

		info.special_key = result;

		has_special_key = true;

		trace_to();

		return true;
	}

	bool parse_header(key_value& info, const std::string& key_world = {})
	{
		std::string key{};
		if (key_world.empty())
		{
			key = trace_key();
		}
		else {
			key = key_world;
		}

		if (key.empty())
			return false;

		if (!check_key(key))
			return false;

		auto value = trace_key();

		info.type = key;
		info.type_name = value;

		trace_to();

		return true;
	}

	bool parse_special_number(message_info& info)
	{
		auto cur = file_content_.peek();

		if (cur != '=')
			return false;

		auto value = trace_key();

		if (value.empty())
			return false;

		value = value.substr(1);

		if (value.empty())
		{
			value = trace_key();
		}

		info.specila_number = value;

		trace_to();

		return true;
	}

	bool parse_start()
	{
		auto value = trace_key('{');

		if (value.size() > 1)
			return false;

		next();

		trace_to();

		return true;
	}

	bool parse_end()
	{
		auto value = trace_key('{');

		if (value.size() > 1)
			return false;

		next();

		trace_to();

		return true;
	}

	bool parse_body(message_info& info)
	{
		auto key = trace_key();

		if (key.empty())
			return true;

		if (switch_key(key, info))
		{
			key = trace_key();
		}

		if (check_key(key))
		{
			info.netests.emplace_back();
			parse_type(info.netests.back(), key);
		}
		else
		{
			auto value = trace_key(';');

			info.keys.push_back({ key,value });
		}
		
		custom_type(key);

		trace_to();

		return parse_body(info);
	}

	bool parse_enum(message_info& info, const std::string& key_world)
	{
		if (!parse_header(info, key_world))
		{
			std::cout << "message is not incomplete\n";

			return false;
		}

		if (!parse_start())
		{
			std::cout << "message not has start flag\n";

			return false;
		}

		if (!parse_enum_body(info))
		{
			std::cout << "enum format error\n";

			return false;
		}

		if (!parse_end())
		{
			std::cout << "message not has end flag\n";

			return false;
		}

		return true;
	}

	bool parse_enum_body(message_info& info)
	{
		auto value = trace_key(';');

		if (value.empty())
			return true;

		info.keys.push_back({ "",value });

		trace_to();

		return parse_enum_body(info);
	}

	bool check_key(const std::string& key)
	{
		auto iter = std::find_if(message_key.begin(), message_key.end(), [&](const auto& k) {return k.data() == key; });

		return iter != message_key.end();
	}

	bool switch_key(const std::string& key, message_info& info)
	{
		auto iter = std::find_if(message_key.begin(), message_key.end(), [&](const auto& k) {return k == key; });

		auto pos = std::distance(message_key.begin(), iter);

		return parse_callback(pos, info, key);
	}

	std::string trace_key(char sp = ' ')
	{
		std::string result{};

		while (!file_content_.eof())
		{
			char cur{};

			if (stop_if(cur, sp, ' ', ';', '{', '}'))
			{
				trip(result, ' ');

				if (cur != '{' && cur != '}')
				{
					file_content_.get();
				}

				return result;
			}

			file_content_.get();

			if (skip_if(cur, '\r', '\n', '\t'))
				continue;

			result.push_back(cur);
		}

		return {};
	}

	template<typename... _Args>
	void trip(std::string& value, _Args&&... args)
	{
		if (value.empty())
			return;

		(value.erase(std::remove(value.begin(), value.end(), args), value.end()), ...);
	}

	bool parse_callback(const std::size_t pos, message_info& info, const std::string& key_world = {})
	{
		if (pos > 1)
			return false;

		if (pos == 0)
		{
			info.netests.push_back({});
			parse_message(info.netests.back(), key_world);
		}

		if (pos == 1)
		{
			info.netests.push_back({});

			parse_enum(info.netests.back(), key_world);
		}

		return true;
	}

	template<typename... _Args>
	bool stop_if(char& cp, _Args&&... args)
	{
		cp = file_content_.peek();

		return ((cp == args) || ...);
	}

	template<typename... _Args>
	bool skip_if(const char& ch, _Args&&... args)
	{
		return ((ch == args) || ...);
	}

	void trace_to()
	{
		while (!file_content_.eof())
		{
			auto cur = file_content_.peek();

			if (!skip_if(cur, ' ', '\r', '\n', '\t'))
				break;

			file_content_.get();
		}
	}

	void next()
	{
		if (file_content_.eof())
			return;

		file_content_.get();
	}

	void custom_type(const std::string& type)
	{
		if (exist(type))
			return;

		auto iter = std::find_if(custom_types_.begin(), custom_types_.end(), [&](const auto t) {return t == type; });

		if (iter != custom_types_.end())
			return;

		custom_types_.push_back(type);
	}

	
private:
	std::string file_name_;

	std::fstream file_content_;

	std::vector<message_info> infos_;

	std::map <std::string, std::function<bool(message_info&, const std::string&)>> struct_invokes_;

	std::vector<std::string> custom_types_;

	std::string syntax_;
};