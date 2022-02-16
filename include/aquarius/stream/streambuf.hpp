#pragma once
#include <array>
#include <algorithm>
#include <streambuf>

namespace aquarius
{
	template<typename T, std::size_t N>
	class streambuf : public std::basic_streambuf<T,std::char_traits<T>>
	{
		using base_type = std::basic_streambuf<T, std::char_traits<T>>;

	public:
		using iterator = std::array<T,N>::iterator;
		using const_iterator = const iterator;
		using value_type = T;
		using size_type = std::size_t;
		using reference = T&;
		using const_reference = const T&;
		using pointer = T*;
		using const_pointer = const pointer;

		streambuf()
			: buffer_()
		{
			reset();
		}

		streambuf(std::span<T> data)
			: streambuf()
		{
			for (auto iter = data.begin(); iter != data.end(); iter++)
			{
				push_back(*iter);
			}
		}

		streambuf(const streambuf& buf)
			: streambuf(std::span<T>(buf.data(),buf.size()))
		{

		}

		virtual ~streambuf() = default;

	public:
		value_type& operator[](size_type pos)
		{
			return buffer_.at(pos);
		}

	public:
		std::size_t size() noexcept
		{
			return base_type::pptr() - base_type::gptr();
		}

		std::size_t size() const noexcept
		{
			return base_type::pptr() - base_type::gptr();
		}

		constexpr std::size_t max_size() noexcept
		{
			return buffer_.max_size();
		}

		pointer data() noexcept
		{
			return base_type::gptr();
		}

		pointer data() const noexcept
		{
			return base_type::gptr();
		}
		
		void clear() noexcept
		{
			buffer_.fill(T{});

			reset();
		}

		template<typename _Ty>
		void push_back(_Ty&& value)
		{
			size() == 0 ? clear() : void();

			constexpr std::size_t sz = sizeof(_Ty);

			std::memcpy(base_type::pptr(), &value, sz);

			commit(sz);
		}

		void commit(int n) noexcept
		{
			n = static_cast<int>(std::min<std::size_t>(n, base_type::epptr() - base_type::pptr()));

			base_type::pbump(n);

			base_type::setg(base_type::eback(), base_type::gptr(), base_type::pptr());
		}

		void consume(int n) noexcept
		{
			if(base_type::egptr() < base_type::pptr())
				base_type::setg(&buffer_[0], base_type::gptr(), base_type::pptr());

			if(base_type::gptr() + n > base_type::pptr())
				n = static_cast<int>(base_type::pptr() - base_type::gptr());

			base_type::gbump(n);
		}

		void reset()
		{
			base_type::setg(&buffer_[0], &buffer_[0], &buffer_[0]);

			base_type::setp(&buffer_[0], &buffer_[0] + buffer_.max_size());
		}

		template<typename _Ty>
		_Ty get()
		{
			_Ty element{};

			constexpr auto I = sizeof(_Ty);

			std::memcpy(&element, reinterpret_cast<_Ty*>(data()), I);

			consume(I);

			return element;
		}

	private:
		std::array<T,N> buffer_;
	};
}
