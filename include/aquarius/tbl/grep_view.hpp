#pragma once
#include <aquarius/concepts.hpp>
#include <aquarius/detail/member_pointer.hpp>
#include <sstream>

namespace aquarius
{
	template <auto Ptr>
	class grep_view
	{
		using value_type = remove_member_pointer<decltype(Ptr)>::type;

		using struct_type = member_pointer_struct<decltype(Ptr)>::type;

		constexpr static std::string_view struct_member_name = get_member_pointer_name<struct_type, Ptr>();

	public:
		grep_view()
			: content_stream_()
			, has_aggr_(false)
		{}

	public:
		grep_view operator()()
		{
			return grep_view<Ptr>();
		}

	public:
		grep_view& operator<(const value_type& g)
		{
			less(g);
			return *this;
		}

		grep_view& operator<=(const value_type& g)
		{
			less_equal(g);
			return *this;
		}

		template <auto Ptr1>
		grep_view& operator>(const grep_view<Ptr1>& g)
		{
			greater(g);
			return *this;
		}

		grep_view& operator>=(const value_type& g)
		{
			greater_equal(g);
			return *this;
		}

		grep_view& operator==(const value_type& g)
		{
			equal(g);
			return *this;
		}

	public:
		grep_view& less(const value_type& g)
		{
			content_stream_.str("");

			content_stream_ << struct_member_name << " < " << g;

			return *this;
		}

		grep_view& less_equal(const value_type& g)
		{
			content_stream_.str("");

			content_stream_ << struct_member_name << " <= " << g;

			return *this;
		}

		grep_view& greater(const value_type& g)
		{
			content_stream_.str("");

			content_stream_ << struct_member_name << " > " << g;
			return *this;
		}

		grep_view& greater_equal(const value_type& g)
		{
			content_stream_.str("");

			content_stream_ << struct_member_name << " >= " << g;

			return *this;
		}

		grep_view& equal(const value_type& g)
		{
			content_stream_.str("");

			content_stream_ << struct_member_name << " = " << g;

			return *this;
		}

		operator std::string() const
		{
			auto result = content_stream_.str();

			if (has_aggr_)
			{
				result = "(" + result + ")";
			}

			return result;
		}

	private:
		template <auto Ptr1, auto Ptr2>
		friend grep_view<Ptr1>& operator||(grep_view<Ptr1>& g1, const grep_view<Ptr2>& g2);

		template <auto Ptr1>
		void _or(const grep_view<Ptr1>& g)
		{
			if (!has_aggr_)
			{
				has_aggr_ = true;
			}

			content_stream_ << " or " << static_cast<std::string>(g);
		}

		void concat(const std::string& sp, const value_type& g)
		{
			content_stream_.str("");

			content_stream_ << struct_member_name << " = ";

			if constexpr (std::same_as<value_type, std::string>)
			{
				content_stream_ << "'";
			}

			content_stream_ << g;

			if constexpr (std::same_as<value_type, std::string>)
			{
				content_stream_ << "'";
			}
		}

	private:
		std::stringstream content_stream_;

		bool has_aggr_;
	};

	template <auto Ptr>
	inline grep_view<Ptr> grep;
} // namespace aquarius