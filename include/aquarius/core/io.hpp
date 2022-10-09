#pragma once
#include <vector>
#include "defines.hpp"

namespace aquarius
{
	template<typename _Ty = uint8_t>
	class io
	{
	public:
		using iterator = std::vector<_Ty>::iterator;
		using const_iterator = const iterator;
		using value_type = _Ty;
		using size_type = std::size_t;
		using reference = _Ty&;
		using const_reference = const _Ty&;
		using pointer = _Ty*;
		using const_pointer = const pointer;

		io()
			: buffer_(buffer_capacity)
			, wpos_(0)
			, rpos_(0)
		{

		}

		io(std::size_t capacity)
			: buffer_(capacity)
			, wpos_(0)
			, rpos_(0)
		{
		}

		io(const io& other)
			: wpos_(other.wpos_)
			, rpos_(other.rpos_)
			, buffer_(other.buffer_)
		{

		}

		io(io&& other)
			: wpos_(std::move(other.wpos_))
			, rpos_(std::move(other.rpos_))
			, buffer_(std::move(other.buffer_))
		{

		}

		virtual ~io() = default;

	public:
		io& operator=(const io& other)
		{
			if (*this != other)
			{
				rpos_ = other.rpos_;
				wpos_ = other.wpos_;
				buffer_ = other.buffer_;
			}

			return *this;
		}

		io& operator=(io&& other)
		{
			wpos_ = std::move(other.wpos_);
			rpos_ = std::move(other.rpos_);
			buffer_.swap(other.buffer_);

			return *this;
		}

	public:
		pointer write_pointer() noexcept
		{
			return buffer_.data() + wpos_;
		}

		pointer read_pointer() noexcept
		{
			return buffer_.data() + rpos_;
		}

		size_type remain_size() noexcept
		{
			return buffer_.size() - wpos_;
		}

		size_type active() noexcept
		{
			return wpos_ - rpos_;
		}

		size_type active() const noexcept
		{
			return wpos_ - rpos_;
		}

		void clear() noexcept
		{
			wpos_ = 0;
			rpos_ = 0;
			buffer_.clear();
		}

		void put(const void* value, size_type sz)
		{
			if (sz <= 0)
				return;

			std::memcpy(write_pointer(), value, sz);

			commit(sz);
		}

		void commit(size_type n) noexcept
		{
			wpos_ += n;
		}

		void consume(size_type n) noexcept
		{
			rpos_ += n;
		}

		void normalize()
		{
			if (rpos_ == 0)
				return;

			std::memmove(buffer_.data(), buffer_.data() + rpos_, wpos_ - rpos_);

			rpos_ = 0;
			wpos_ -= rpos_;
		}

		void ensure()
		{
			if (buffer_.size() != wpos_)
				return;

			buffer_.resize(buffer_.size() * 2);
		}

		void resize(size_type sz)
		{
			if (sz <= buffer_.size())
				return;

			buffer_.resize(sz);
		}

		size_type size()
		{
			return buffer_.size();
		}

		template<typename _Ty>
		_Ty get()
		{
			_Ty value{};

			constexpr auto sz = sizeof(_Ty);

			std::memcpy(&value, &buffer_[rpos_], sz);

			consume(sz);

			return value;
		}

	private:
		size_type wpos_;

		size_type rpos_;

		std::vector<_Ty> buffer_;
	};
}
