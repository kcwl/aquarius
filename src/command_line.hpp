#pragma once
#include <string>


class command_line
{
public:
	command_line() = default;


public:
	std::string input_file_name_;

	std::string output_file_directory_;

	std::string output_type_;

	std::string output_source_type_;
};