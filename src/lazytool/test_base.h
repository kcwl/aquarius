#pragma once
#include <fstream>
#include <string>

namespace aquarius
{
	namespace virgo
	{
		class test_base
		{
		public:
			void write_equal(std::fstream& ofs, const std::string& name);

			void write_stream(std::fstream& ofs, const std::string& name);

			virtual void equal_impl(std::fstream& ofs) = 0;

			virtual void stream_impl(std::fstream& ofs) = 0;
		};
	}
}