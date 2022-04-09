#pragma once
#include <span>
#include "streambuf.hpp"
#include "../detail/algorithm.hpp"

namespace aquarius
{
	inline constexpr static std::size_t capacity = 4 * 1024;

	template<std::size_t I = capacity, typename _T = char>
	class basic_ftstream
	{
	public:
		basic_ftstream()
			: buffer_()
		{
		}

		basic_ftstream(streambuf<_T, I> ios)
			: buffer_(ios)
		{

		}

		basic_ftstream(std::span<_T> data)
		{
			for (auto iter : data)
			{
				push(iter);
			}
		}

	public:
		template<typename T>
		basic_ftstream& operator << (T&& value)
		{
			push(std::move(value));
			return *this;
		}

		template<typename T>
		basic_ftstream& operator >> (T& value)
		{
			value = pop<T>();
			return *this;
		}

		template<typename T>
		T cat()
		{
			T result{};

			constexpr auto sz = sizeof(T);

			basic_ftstream bf(std::span<_T>(data(), sz));

			bf >> result;

			return result;
		}

		void clear()
		{
			return buffer_.clear();
		}

		streambuf<_T, I>::value_type* data() noexcept
		{
			return buffer_.data();
		}

		streambuf<_T, I>::value_type* data() const noexcept
		{
			return buffer_.data();
		}

		std::size_t size() noexcept
		{
			return buffer_.size();
		}

		constexpr std::size_t size() const noexcept
		{
			return buffer_.size();
		}

		constexpr std::size_t max_size() noexcept
		{
			return buffer_.max_size();
		}

		void commit(int n) noexcept
		{
			return buffer_.commit(n);
		}

		void consume(int length)
		{
			return buffer_.consume(length);
		}

		void reset(std::size_t n) noexcept
		{
			return buffer_.reset(n);
		}

	private:
		template<typename _Ty>
			requires(is_pod_v<std::remove_cvref_t<_Ty>>)
		void push(_Ty&& value)
		{
			detail::for_each(std::move(value), [&](auto&& iter)
					 {
						 buffer_.push_back(iter);
					 });
		}

		template<typename _Ty>
			requires(is_container_v<_Ty>)
		void push(_Ty&& value)
		{
			std::size_t size = value.size();

			push(size);

			for (auto iter : value)
			{
				push(iter);
			}
		}

		template<typename _Ty>
		void push(_Ty&& value)
		{
			detail::for_each(std::move(value), [&](auto&& iter)
					 {
						 push(iter);
					 });
		}

		template<std::size_t I, typename _Ty>
		auto make_element()
		{
			auto element = reflect::get<I>(_Ty{});

			using element_t = decltype(element);

			if constexpr (!std::is_trivial_v<element_t>)
			{
				return pop<element_t>();
			}
			else
			{
				return buffer_.get<element_t>();
			}
		}

		template<typename _Ty, std::size_t... I>
		auto pop_impl(std::index_sequence<I...>)
		{
			return _Ty{ make_element<I,_Ty>()... };
		}

		template<typename _Ty, std::size_t N = reflect::rf_size_v<_Ty>, typename Indices = std::make_index_sequence<N>>
		_Ty pop()
		{
			return pop_impl<_Ty>(Indices{});
		}

		template<typename _Ty>
			requires(is_container_v<_Ty>)
		_Ty pop()
		{
			_Ty element{};

			std::size_t element_size = buffer_.get<std::size_t>();

			using element_type = _Ty::value_type;

			for (int32_t i = 0; i != element_size; ++i)
			{
				auto ele = pop<element_type>();

				element.push_back(ele);
			}

			return element;
		}

	private:
		streambuf<_T, I> buffer_;
	};
}