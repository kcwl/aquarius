#include "registor.h"
#include "log.h"
#include <system_error>

namespace aquarius
{
	namespace virgo
	{
		void registor::regist(std::string struct_name, func_t&& func)
		{
			invokes_[struct_name] = func;
		}

		auto registor::invoke(std::string struct_name, std::size_t column, std::size_t row) -> registor::func_t
		{
			auto iter = invokes_.find(struct_name);
			if (iter == invokes_.end())
				throw std::runtime_error(log("main", "syntax error! missing protocol or struct keyword!", column, row));

			return invokes_[struct_name];
		}
	} // namespace virgo
} // namespace aquarius
