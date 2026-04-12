#pragma once
#include <aquarius/detail/concat.hpp>
#include <aquarius/detail/member_pointer.hpp>

namespace aquarius
{
	template <typename T, auto... Args>
	class select_view
	{
	public:
		constexpr static auto make_member_names()
		{
			constexpr static auto sp = ","sv;
			auto f = [] { return concat_v<(member_pointer_name<T, Args>::value, sp)...>; };

			constexpr static auto result = f();

			return result.substr(0, result.size() - 1);
		}

		constexpr static auto membes = make_member_names();

		constexpr static auto make_content()
		{
			constexpr static auto name = detail::struct_name<T>();
			constexpr static auto sel = "select * from "sv;

			constexpr static auto part_sel = "select "sv;
			constexpr static auto from = " from "sv;

			if constexpr (sizeof...(Args) == 0)
			{
				return concat_v<sel, name>;
			}
			else
			{
				return concat_v<part_sel, membes, from, name>;
			}
		}

		constexpr static auto content = make_content();

	public:
		select_view()
			: has_condition_(false)
		{}

		operator std::string() const
		{
			return std::string(content) + content_stream_.str();
		}

	private:
		template <typename T1, auto Ptr, auto... Args1>
		friend select_view<T1, Args1...>& operator|(select_view<T1, Args1...>& sel, const grep_view<Ptr>& g);

	private:
		template <auto Ptr>
		void merge(const grep_view<Ptr>& g)
		{
			if (!has_condition_)
			{
				content_stream_ << " where";

				has_condition_ = true;
			}

			std::cout << "debug:" << static_cast<std::string>(g) << std::endl;

			content_stream_ << " and " << static_cast<std::string>(g);
		}

	private:
		bool has_condition_;

		std::stringstream content_stream_;
	};

	template <typename T, auto... Args>
	inline static select_view<T, Args...> select;
} // namespace aquarius