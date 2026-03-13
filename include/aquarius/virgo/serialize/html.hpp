#pragma once
#include <aquarius/serialize/flex_buffer.hpp>
#include <string>

namespace aquarius
{
	struct html_parse
	{
		void to_datas(const std::string& str, flex_buffer& buffer)
		{
			buffer.sputn(str.c_str(), str.size());
		}

		std::string from_datas(flex_buffer& buffer)
		{
			std::string stream{};

			stream.resize(buffer.size());

			buffer.sgetn(stream.data(), stream.size());
			return stream;
		}
	};
} // namespace aquarius