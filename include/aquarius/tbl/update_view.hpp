#pragma once
#include <sstream>

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

			using type = std::remove_cvref_t<T>;

			// constexpr auto struct_name = detail::struct_name<type>();
			constexpr auto struct_name = "person"sv;

			complete_sql_ << "update " << struct_name;

			constexpr static auto size = boost::pfr::tuple_size_v<type>;

			auto f = [&]<std::size_t... I>(std::index_sequence<I...>)
			{
				((complete_sql_ << (I == 0 ? " set " : " and ") << boost::pfr::get_name<I, type>() << "="
								<< boost::pfr::get<I, type>(value)),
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
	};

	static update_view update;
} // namespace aquarius