#include "generate_src_helper.h"

namespace
{
	constexpr static auto CRLF = "\r\n";
}

namespace aquarius
{
	namespace lazytool
	{
		void genrate_src(std::ofstream& ofs, const std::string& class_name, const data_field& fd, const std::string& method,
						 const std::string& direction, bool has_type)
		{
			if (method == "binary")
			{
				for (auto& [type, name] : fd.fields())
				{
					if (has_type)
					{
						ofs << "\tthis->parse_" << direction << "<" << type << ">(buffer);" << CRLF;
					}
					else
					{
						ofs << "\tthis->parse_" << direction << "(" << name << ", buffer);" << CRLF;
					}
				}
			}
			else if (method == "kv")
			{
				bool start = true;
				for (auto& [type, name] : fd.fields())
				{
					if (start)
					{
						start = !start;
						ofs << "\tbuffer.put('?');" << CRLF;
					}
					else
						ofs << "\tbuffer.put('&');" << CRLF;

					if (has_type)
					{
						ofs << "\tthis->parse_" << direction << "<" << type << ">(buffer, \"" << name << "\");" << CRLF;
					}
					else
					{
						ofs << "\tthis->parse_" << direction << "(" << name << ", buffer, \"" << name << "\");" << CRLF;
					}
				}

				ofs.seekp(-1, std::ios::cur);
				ofs << " ";
			}
			else if (method == "json")
			{
				if (has_type)
				{
					ofs << "\t*this = this->parse_" << direction << "<" << class_name << ">(buffer); " << CRLF;
				}
				else
				{
					ofs << "\tthis->parse_" << direction << "(*this, buffer);" << CRLF;
				}
			}
			else
			{
			}
		}
	}
}