#include "test_base.h"
namespace aquarius
{
	namespace virgo
	{
		void test_base::write_equal(std::fstream& ofs, const std::string& name)
		{
			ofs << "\tbool operator==(const " << name << "& lhs, const " << name << "& rhs)\n";

			ofs << "\tfriend inline bool operator==(const " << name << "& lhs, const " << name << "& rhs)\n";
			ofs << "\t{\n";
			ofs << "\t\treturn ";
			//for (auto& [_, name] : scopes_)
			//{
			//	ofs << "lhs." << name << " == rhs." << name << " && ";
			//}

			//ofs.seekg(-4, std::ios::cur);
			//ofs << ";\n";

			equal_impl(ofs);

			ofs << "\t}\n\n";
		}

		void test_base::write_stream(std::fstream& ofs, const std::string& name)
		{
			ofs << "std::ostream& operator<<(std::ostream& os, const " << name << "_req_header& other)\n";
			ofs << "{\n";
			//request_.generate_stream(ofs, router_.type(), "header");
			stream_impl(ofs);
			ofs << "}\n\n";
		}
	}
}