// lazytool.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "file_outer.hpp"

auto parse_by_space(int args, char** argc) -> std::map<std::string, std::string>
{
	std::map<std::string, std::string> key_value;

	std::string last_key{};

	for (int i = 0; i < args; ++i)
	{
		if (argc[i][0] == '-')
		{
			last_key = argc[i];

			key_value[last_key] = "";

			continue;
		}

		key_value[last_key] = argc[i];

		last_key.clear();
	}

	return key_value;
}

bool parse_command(int args, char** argc, command_line& line)
{
	auto key_value = parse_by_space(args, argc);

	if (key_value.empty())
		return false;

	for (const auto& [key, value] : key_value)
	{
		if (key == "--input")
		{
			auto suffix = value.substr(value.find_last_of(".") + 1);

			if (suffix != suffix_str)
			{
				std::cout << "file suffix is error\n";

				return false;
			}

			line.input_file_name_ = value.substr(0, value.find_last_of("."));

		}
		else if (key == "--cpp_out")
		{
			line.output_type_ = "h";
			line.output_source_type_ = "cpp";
			line.output_file_directory_ = value;
		}
	}

	return true;
}


int main(int args, char** argc)
{
	if (args < 2)
	{
		std::cout << "maybe params list is unknown, please use \"--help\" or \"-h\" to understand usage!\n";

		return 0;
	}


	command_line line{};

	if (!parse_command(args, argc, line))
	{
		std::cout << "maybe params list is unknown, please use \"--help\" or \"-h\" to understand usage!\n";

		return 0;
	}

	file_descriper fd{ line };

	if (!fd.load())
	{
		return  0;
	}

	if (!fd.parse())
	{
		return 0;
	}

	file_outer fo(line, fd);

	fo.outer(fd.get_message_infos());

	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
