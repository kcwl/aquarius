#pragma once
#include <string>
#include <algorithm>
#include <streambuf>
#include <reflect.hpp>
#include "type_traits.hpp"

namespace aquarius
{
	namespace detail
	{
		template<typename Alloc = std::allocator<char>>
		class stream_buf : public std::streambuf
		{
			inline constexpr static std::size_t buffer_capacity = 4 * 1024;
		public:
			explicit stream_buf(std::size_t size = buffer_capacity, const Alloc& allocator = Alloc{})
				: buffer_(allocator)
				, max_size_(size)
			{
				resize(max_size_);
			}

			virtual ~stream_buf() = default;

		public:
			std::size_t size() const noexcept
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

			char_type* data() const noexcept
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

			template<typename _Ty>
			void push_back(_Ty&& value)
			{
				push(std::forward<_Ty>(value));
			}

			template<typename _Ty>
			auto pop_front() -> _Ty
			{
				return pop<_Ty>();
			}

		public:
			template<typename T>
			stream_buf& operator << (T&& value)
			{
				push_back(std::move(value));
				return *this;
			}

			template<typename T>
			stream_buf& operator >> (T& value)
			{
				value = pop_front<T>();
				return *this;
			}

		private:
			template<typename _Ty>
			requires(!is_container_v<_Ty>)
			auto push_impl(_Ty&& val)
			{
				_Ty tmp = std::forward<_Ty>(val);
				std::memcpy(pptr(), reinterpret_cast<_Ty*>(&tmp), sizeof(_Ty));

				commit(sizeof(_Ty));
			}

			template<typename _Ty>
			requires(is_container_v<_Ty>)
			auto push(_Ty&& val)
			{
				int32_t size = static_cast<int32_t>(val.size());
				push_impl(std::move(size));
				std::for_each(val.begin(), val.end(), [this] (auto iter)
					{
						push(std::move(iter));
					});
			}

			template<typename _Ty>
			requires(!is_container_v<_Ty>)
			auto push(_Ty&& val)
			{
				for_each(std::move(val), [this] (auto&& iter)
					{
						push_impl(std::move(iter));
					});
			}

			template<std::size_t I, typename _Ty>
			requires(!is_container_v<_Ty>)
			auto get()
			{
				auto element = reflect::rf_element<I>(_Ty{});

				using element_t = decltype(element);

				constexpr auto N = sizeof(element_t);

				std::memcpy(&element, reinterpret_cast<element_t*>(gptr()), N);

				consume(N);

				return element;
			}

			template<typename _Ty, std::size_t... I>
			auto pop_impl(std::index_sequence<I...>)
			{
				return _Ty{get<I,_Ty>()...};
			}

			template<typename _Ty, std::size_t N = reflect::rf_size_v<_Ty>, typename Indices = std::make_index_sequence<N>>
			requires(!is_container_v<_Ty>)
			auto pop()
			{
				return pop_impl<_Ty>(Indices{});
			}

			template<typename _Ty>
			requires(is_container_v<_Ty>)
			auto pop()
			{
				_Ty element{};

				int32_t element_size{};

				constexpr std::size_t  N = sizeof(int32_t);

				using element_type = _Ty::value_type;

				std::memcpy(&element_size, reinterpret_cast<int32_t*>(gptr()), N);

				consume(N);

				for(int32_t i = 0; i != element_size; ++i)
				{
					auto ele = pop<element_type>();

					element.push_back(ele);
				}

				return element;
			}

			template<typename _Ty, typename Func, std::size_t... I>
			constexpr void for_each_impl(_Ty&& val, Func func, std::index_sequence<I...>)
			{
				return (func(reflect::rf_element<I>(std::forward<_Ty>(val))), ...);
			}

			template<typename _Ty, typename Func, std::size_t N = reflect::rf_size_v<_Ty>, typename Indices = std::make_index_sequence<N>>
			constexpr void for_each(_Ty&& val, Func func)
			{
				return for_each_impl(std::forward<_Ty>(val), func, Indices{});
			}

		private:
			std::vector<char_type, Alloc> buffer_;
			std::size_t max_size_;
		};

		using streambuf = stream_buf<>;
	}
}
