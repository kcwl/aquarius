#pragma once
#include <string>
#include <map>
#include "defines.h"

namespace aquarius
{
	struct note
	{
		std::string content_;
		note_type type_;
	};

	struct reflactor_structure
	{
		std::string number_;

		std::string type_;

		std::string name_;

		std::multimap<std::string, std::string> keywords_;
	};
}