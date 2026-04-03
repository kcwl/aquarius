#pragma once
#include <sstream>

namespace aquarius
{
	class update_view
	{
	public:
		template <typename T>
		update_view& operator()(T&& value)
		{
			complete_sql_.str("");

			constexpr auto struct_name = aquarius::detail::struct_name<std::remove_cvref_t<T>>();

			complete_sql_ << "update " << struct_name;

			return *this;
		}

		operator std::string() const
		{
			return complete_sql_.str();
		}

	private:
		std::stringstream complete_sql_;
	};

	static update_view update;
} // namespace aquarius