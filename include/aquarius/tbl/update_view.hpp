#pragma once
#include <aquarius/detail/struct_name.hpp>
#include <aquarius/tbl/add_string.hpp>
#include <boost/pfr.hpp>
#include <sstream>

template <auto Ptr>
class grep_view;

namespace aquarius
{
	class update_view
	{
	public:
		update_view()
			: complete_sql_()
			, has_condition_(false)
		{}

	public:
		template <typename T>
		update_view& operator()(T&& value)
		{
			complete_sql_.str("");
			has_condition_ = false;

			using type = std::remove_cvref_t<T>;

			constexpr auto struct_name = detail::struct_name<type>();

			complete_sql_ << "update " << struct_name;

			constexpr static auto size = boost::pfr::tuple_size_v<type>;

			auto f = [&]<std::size_t... I>(std::index_sequence<I...>)
			{
				((complete_sql_ << (I == 0 ? " set " : " and ") << boost::pfr::get_name<I, type>() << " = "
								<< add_string(boost::pfr::get<I, type>(value))),
				 ...);
			};

			f(std::make_index_sequence<size>());

			return *this;
		}

		operator std::string() const
		{
			return complete_sql_.str();
		}

	private:
		template <auto Ptr>
		friend update_view& operator|(update_view& up, const grep_view<Ptr>& g);

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
	inline update_view& operator|(update_view& up, const grep_view<Ptr>& g)
	{
		up.merge(g);
		return up;
	}

	inline static update_view update_v;
} // namespace aquarius