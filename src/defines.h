#pragma once
#include <string>

namespace aquarius
{
	enum class file_status
	{
		success,
		format_error,
	};

	enum class note_dir
	{
		left,
		right
	};

	enum class note_type
	{
		double_slash,
		slash
	};

	inline static const std::string tab = "\t";
	inline static const std::string crlf = "\r\n";

	inline static const std::string file_suffix = "proto";
} // namespace elastic