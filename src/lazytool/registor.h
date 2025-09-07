#pragma once
#include <functional>
#include <map>
#include <string>

namespace virgo
{
	class registor
	{
		using func_t = std::function<void(std::fstream& ifs)>;

	public:
		registor() = default;

	public:
		void regist(std::string struct_name, func_t&& func);

		auto invoke(std::string struct_name, std::size_t column, std::size_t row) -> func_t;

	private:
		std::map<std::string, func_t> invokes_;
	};
} // namespace virgo