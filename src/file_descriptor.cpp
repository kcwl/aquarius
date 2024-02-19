#include "file_descriptor.h"

#include "common.hpp"

#include <iostream>
#include <vector>
#include <deque>
#include <filesystem>

namespace aquarius
{
	namespace compiler
	{
		bool file_descriptor::read_file(const std::string& file_name)
		{
			if (!check_file_suffix(file_name))
			{
				std::cout << "file format error, maybe suffix is not \"."<< file_suffix <<"\"!";

				return false;
			}

			read_file_stream_.open(file_name, std::ios::in | std::ios::binary);
			if (!read_file_stream_.is_open())
			{
				std::cout << "maybe file is not exist!";

				return false;
			}

			while (!read_file_stream_.eof())
			{
				reflactor_structure impl{};

				if (!read_keyword(impl.type_))
					continue;

				if (!check_key_info(impl.type_))
					continue;

				if (impl.type_ == "message")
				{
					if (!read_message(impl))
						continue;
				}
				else if (impl.type_ == "package")
				{
					if (!read_package(impl))
						continue;
				}

				multi_key_words_.push_back(impl);
			}

			return true;
		}

		std::string file_descriptor::file_name() const
		{
			return input_file_name_;
		}

		std::vector<reflactor_structure> file_descriptor::structs() const
		{
			return multi_key_words_;
		}

		bool file_descriptor::read_to_spilt(std::string& value, int sp)
		{
			auto cur = read_file_stream_.peek();

			if (cur == sp)
			{
				read_file_stream_.get();

				return true;
			}

			std::array<char, 1024> temp_line{};

			read_file_stream_.get(&temp_line[0], 1024, (char)sp);

			value = temp_line.data();

			if (!read_file_stream_.eof())
			{
				read_file_stream_.get();
			}

			return !read_file_stream_.fail();
		}

		bool file_descriptor::read_command(std::string& name)
		{
			if (!read_to_spilt(name, ';'))
				return false;

			skip_if(read_file_stream_, '\r', '\n','\t',' ');

			return true;
		}

		bool file_descriptor::check_file_suffix(const std::string& file_name)
		{
			auto slash_pos = file_name.find_last_of("/\\");

			slash_pos == std::string::npos ? slash_pos = 0 : slash_pos += 1;

			auto pos = file_name.find_last_of(".");
			if (pos == std::string::npos)
				return false;

			input_file_name_ = file_name.substr(slash_pos, pos - slash_pos);

			auto suffix = file_name.substr(pos + 1);

			if (suffix.compare(file_suffix) != 0)
				return false;

			return true;
		}

		bool file_descriptor::read_keyword(std::string& keyword)
		{
			if (!read_to_spilt(keyword, ' '))
				return false;

			keyword = spilt_keyword_from_line(keyword);

			if (keyword.empty())
				return false;

			return true;
		}

		bool file_descriptor::read_keytype(std::string& keytype)
		{
			if (!read_to_spilt(keytype, ' ') && !read_file_stream_.eof())
				return false;

			auto pos = keytype.find("}");

			if (pos != std::string::npos)
				return true;

			keytype = spilt_keytype_from_line(keytype);

			if (keytype.empty())
				return false;

			return true;
		}

		bool file_descriptor::read_message(reflactor_structure& rs)
		{
			std::string name_and_number{};

			read_to_spilt(name_and_number, '{');

			trip(name_and_number, '\r', '\n', '\t', ' ');

			if (!spilt_by(name_and_number, '=', rs.name_, rs.number_))
				return false;

			read_file_stream_.get();

			while (!read_file_stream_.eof())
			{
				std::string type{};
				std::string name{};

				if (!read_keytype(type))
					return false;

				auto pos = type.find("}");

				if (pos != std::string::npos)
				{
					int off = static_cast<int>(type.size() - pos - 1);

					read_file_stream_.seekg(-off, std::ios::cur);

					break;
				}

				if (!read_command(name))
					return false;

				rs.keywords_.insert({type,name});
			}

			return true;
		}

		bool file_descriptor::read_package(reflactor_structure& rs)
		{
			auto pos = read_file_stream_.tellg();

			if (!read_command(rs.name_))
				return false;

			if (check_space(rs.name_))
			{
				read_file_stream_.seekg(pos);

				std::string str{};

				read_to_spilt(str, ' ');

				return false;
			}
				
			return true;
		}

		std::string file_descriptor::spilt_keyword_from_line(const std::string& keyword)
		{
			for (auto& key : key_word)
			{
				auto pos = keyword.find(key.first);

				if (pos == std::string::npos)
					continue;

				if (pos > 0)
				{
					auto& cur = keyword[pos - 1];
					if ((cur != '\r') && (cur != '\n') && (cur != ' ') && (cur != '\t'))
						return {};
				}

				return key.first;
			}

			return {};
		}

		std::string file_descriptor::spilt_keytype_from_line(const std::string& keytype)
		{
			for (auto& key : key_type)
			{
				auto pos = keytype.find(key.first);

				if (pos == std::string::npos)
					continue;

				if (pos > 0)
				{
					auto& cur = keytype[pos - 1];
					if ((cur != '\r') && (cur != '\n') && (cur != ' ') && (cur != '\t'))
						continue;
				}

				return key.first;
			}

			std::string result = keytype;

			trip(result, '\r', '\n', '\t', ' ');

			return result;
		}

		bool file_descriptor::check_space(const std::string& word)
		{
			for (auto& s : word)
			{
				if (!std::isspace(s))
					continue;

				return true;
			}

			return false;
		}
	} // namespace compiler
} // namespace elastic