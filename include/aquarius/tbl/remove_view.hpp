#pragma once
#include <sstream>

namespace aquarius
{
	class remove_view
	{
	public:
		template <typename T>
		remove_view& operator()(T&& value)
		{
			constexpr auto struct_name = aquarius::detail::struct_name<std::decay_t<T>>();

			complete_sql_.str("");

			complete_sql_ << "delete from " << struct_name;

			return *this;
		}

		operator std::string() const
		{
			return complete_sql_.str();
		}

	private:
		std::stringstream complete_sql_;
	};
	static remove_view remove;
} // namespace aquarius