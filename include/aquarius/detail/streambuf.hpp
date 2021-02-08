#pragma once
#include <string>
#include <algorithm>
#include <streambuf>
#include <reflect.hpp>
#include "type_traits.hpp"



template<class Alloc = std::allocator<char>>
class streambuf : public std::streambuf
{
	inline constexpr static std::size_t buffer_capacity = 4 * 1024;
public:
	explicit streambuf(std::size_t size = buffer_capacity, const Alloc& allocator = Alloc{})
		: buffer_(allocator)
		, max_size_(size,buffer_capacity)
	{
		resize(max_size_);
	}

	streambuf(const char* val, std::size_t length)
	{
		std::for_each(std::begin(val), std::end(val), [this] (auto iter)
			{
				buffer_.push_back(iter);
			});

		commit(length);
	}

	virtual ~streambuf() = default;

public:
	std::size_t size() noexcept
	{
		return pptr() - gptr();
	}

	std::size_t max_size() noexcept
	{
		return max_size_;
	}

	void resize(std::size_t n) noexcept
	{
		buffer_.resize(n);
		setg(&buffer_[0], &buffer_[0], &buffer_[0]);
		setp(&buffer_[0], &buffer_[0] + n);
	}

	char_type* data() noexcept
	{
		return gptr();
	}

	void commit(std::size_t n) noexcept
	{
		n = std::min<std::size_t>(n, epptr() - pptr());
		pbump(static_cast<int>(n));
		setg(eback(), gptr(), pptr());
	}

	void consume(std::size_t n) noexcept
	{
		if (egptr() < pptr())
			setg(&buffer_[0], gptr(), pptr());

		if (gptr() + n > pptr())
			n = pptr() - gptr();

		gbump(static_cast<int>(n));
	}

	template<class _Ty>
	requires(!is_container_v<_Ty>)
	void push_back(_Ty&& value)
	{
		for_each(std::move(value), [this] (auto&& iter)
			{
				push(std::move(iter));
			});
	}

	template<class _Ty>
	requires(is_container_v<_Ty>)
	void push_back(_Ty&& value)
	{
		push(std::forward<_Ty>(value));
	}

	template<class _Ty>
	requires(!is_container_v<_Ty>)
	auto pop_front() -> _Ty
	{
		return pop<_Ty>();
	}

	template<class _Ty>
	requires(is_container_v<_Ty>)
	auto pop_front() -> _Ty
	{
		return get<0, _Ty>();
	}

private:
	template<class _Ty>
	auto push(_Ty&& val)
	{
		if constexpr (!is_container_v<_Ty>)
		{
			_Ty tmp = std::forward<_Ty>(val);
			std::memcpy(pptr(), reinterpret_cast<_Ty*>(&tmp), sizeof(_Ty));

			commit(sizeof(_Ty));
		}
		else
		{
			int32_t size = static_cast<int32_t>(val.size());
			push(std::move(size));
			std::for_each(val.begin(), val.end(), [this] (auto iter)
				{
					push(std::move(iter));
				});
		}
	}

	template<std::size_t I, class _Ty>
	auto get()
	{
		auto element = reflect::rf_element<I>(_Ty{});

		using element_t = decltype(element);

		if constexpr (!is_container_v<element_t>)
		{
			constexpr auto N = sizeof(element_t);

			std::memcpy(&element, reinterpret_cast<element_t*>(gptr()), N);

			consume(N);
		}
		else
		{
			int32_t element_size{};

			constexpr std::size_t  N = sizeof(int32_t);

			std::memcpy(&element_size, reinterpret_cast<std::size_t*>(gptr()), N);

			consume(N);

			for (int32_t i = 0; i != element_size; ++i)
			{
				auto ele = pop<element_t::value_type>();

				element.push_back(ele);
			}
		}

		return element;
	}

	template<class _Ty, std::size_t... I>
	auto pop(std::index_sequence<I...>)
	{
		return _Ty{get<I,_Ty>()...};
	}

	template<class _Ty, std::size_t N = reflect::rf_size_v<_Ty>, class Indices = std::make_index_sequence<N>>
	auto pop()
	{
		return pop<_Ty>(Indices{});
	}

	template<class _Ty, class Func, std::size_t... I>
	constexpr void for_each(_Ty&& val, Func func, std::index_sequence<I...>)
	{
		return (func(reflect::rf_element<I>(std::forward<_Ty>(val))), ...);
	}

	template<class _Ty, class Func, std::size_t N = reflect::rf_size_v<_Ty>, class Indices = std::make_index_sequence<N>>
	constexpr void for_each(_Ty&& val,Func func)
	{
		return for_each(std::forward<_Ty>(val), func, Indices{});
	}

private:
	std::vector<char_type, Alloc> buffer_;
	std::size_t max_size_;
};