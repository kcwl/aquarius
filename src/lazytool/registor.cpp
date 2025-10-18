#include "registor.h"
#include "log.h"
#include <system_error>
#include <format>

namespace aquarius
{
	namespace lazytool
	{
		void registor::regist(std::string struct_name, func_t&& func)
		{
			invokes_[struct_name] = func;
		}

		auto registor::invoke(const std::string& struct_name, std::size_t column, std::size_t row) -> registor::func_t
		{
			auto iter = invokes_.find(struct_name);
			if (iter == invokes_.end())
				throw std::runtime_error(log(
					"main", std::format("syntax error [{}]! missing protocol or struct or enum keyword!", struct_name),
					column, row));

			return invokes_[struct_name];
		}
	} // namespace lazytool
} // namespace aquarius
