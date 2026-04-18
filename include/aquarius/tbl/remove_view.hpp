#pragma once
#include <aquarius/detail/struct_name.hpp>
#include <sstream>

namespace aquarius
{
	template<auto Ptr>
	class grep_view;

	template <typename T>
	class remove_view
	{
	public:
		remove_view& operator()()
		{
			reset();

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
		template <auto Ptr, typename U>
		friend remove_view<U>& operator|(remove_view<U>& up, const grep_view<Ptr>& g);

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

		void reset()
		{
			complete_sql_.str("");

			has_condition_ = false;
		}

	private:
		std::stringstream complete_sql_;

		bool has_condition_;
	};

	template <typename T>
	inline static remove_view<T> remove_v;
} // namespace aquarius