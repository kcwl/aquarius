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
		template <auto Ptr>
		friend remove_view& operator|(remove_view& up, const grep_view<Ptr>& g);

		template <auto Ptr>
		void merge(const grep_view<Ptr>& g)
		{
			if (!has_condition_)
			{
				complete_sql_ << " where ";

				has_condition_ = true;
			}
			else
			{
				complete_sql_ << " and ";
			}

			complete_sql_ << static_cast<std::string>(g);
		}

	private:
		std::stringstream complete_sql_;

		bool has_condition_;
	};

	template <auto Ptr>
	inline remove_view& operator|(remove_view& up, const grep_view<Ptr>& g)
	{
		up.merge(g);

		return up;
	};

	static remove_view remove;
} // namespace aquarius