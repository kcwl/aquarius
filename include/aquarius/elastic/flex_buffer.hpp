#pragma once
#include <cstring>
#include <ios>
#include <iterator>
#include <span>
#include <streambuf>
#include <vector>

namespace aquarius
{
	template <typename _Elem, typename _Traits, typename _Alloc = std::allocator<_Elem>>
	class flex_buffer
	{
		using allocator_type = _Alloc;

		using traits_type = _Traits;

		constexpr static std::size_t capacity = 4096;

		constexpr static std::size_t water_line = 32;

	public:
		using iterator = typename std::vector<_Elem, allocator_type>::iterator;
		using const_iterator = typename std::vector<_Elem, allocator_type>::const_iterator;
		using value_type = typename std::vector<_Elem, allocator_type>::value_type;
		using size_type = typename std::vector<_Elem, allocator_type>::size_type;
		using reference = typename std::vector<_Elem, allocator_type>::reference;
		using const_reference = typename std::vector<_Elem, allocator_type>::const_reference;
		using pointer = typename std::vector<_Elem, allocator_type>::pointer;
		using const_pointer = std::vector<_Elem, allocator_type>::const_pointer;

		using off_type = typename traits_type::off_type;
		using pos_type = typename traits_type::pos_type;
		using int_type = typename traits_type::int_type;

	public:
		flex_buffer()
			: flex_buffer(capacity)
		{}

		flex_buffer(size_type number)
			: buffer_(number)
			, pptr_(0)
			, gptr_(0)
		{}

		template <typename _Iter>
		flex_buffer(_Iter begin, _Iter end)
			: flex_buffer()
		{
			buffer_.clear();

			std::copy(begin, end, std::back_inserter(buffer_));

			commit(static_cast<int>(buffer_.size()));
		}

		template <typename _Ty, std::size_t N, typename = std::is_convertible<_Ty, _Elem>>
		flex_buffer(std::span<_Ty, N> data)
			: flex_buffer(data.begin(), data.end())
		{
			static_assert(N <= capacity, "maybe input is too large!");
		}

		flex_buffer(flex_buffer&& other)
		{
			if (this != std::addressof(other))
			{
				flex_buffer().swap(other);
			}
		}

		flex_buffer(const void* buffer, size_type sz)
			: flex_buffer()
		{
			auto active_sz = active();

			sz > active_sz ? sz = active_sz : 0;

			traits_type::copy(rdata(), buffer, sz);

			commit(static_cast<int>(sz));
		}

		flex_buffer(const flex_buffer&) = delete;
		flex_buffer& operator=(const flex_buffer&) = delete;

		bool operator==(const flex_buffer& other) const
		{
			return buffer_ == other.buffer_ && pptr_ = other.pptr_ && gptr_ == other.gptr_;
		}

	public:
		size_type active() noexcept
		{
			return buffer_.size() - static_cast<std::size_t>(pptr_);
		}

		size_type active() const noexcept
		{
			return buffer_.size() - static_cast<std::size_t>(pptr_);
		}

		pointer data() noexcept
		{
			return buffer_.data();
		}

		const_pointer data() const noexcept
		{
			return buffer_.data();
		}

		pointer wdata() noexcept
		{
			return data() + gptr_;
		}

		const_pointer wdata() const noexcept
		{
			return data() + gptr_;
		}

		pointer rdata() noexcept
		{
			return data() + pptr_;
		}

		const_pointer rdata() const noexcept
		{
			return data() + pptr_;
		}

		void commit(off_type bytes)
		{
			bytes = std::min<off_type>(bytes, static_cast<off_type>(buffer_.size()) - pptr_);

			pptr_ += bytes;

			pptr_ < 0 ? pptr_ = 0 : 0;
		}

		void consume(off_type bytes)
		{
			bytes = std::min<off_type>(bytes, pptr_ - gptr_);

			gptr_ += bytes;

			gptr_ < 0 ? gptr_ = 0 : 0;
		}

		constexpr iterator begin() noexcept
		{
			return buffer_.begin();
		}

		constexpr const_iterator begin() const noexcept
		{
			return buffer_.begin();
		}

		constexpr iterator end() noexcept
		{
			return buffer_.end();
		}

		constexpr const_iterator end() const noexcept
		{
			return buffer_.end();
		}

		void clear() noexcept
		{
			buffer_.clear();

			pptr_ = 0;

			gptr_ = 0;
		}

		void resize(size_type bytes)
		{
			clear();

			buffer_.resize(bytes);
		}

		void swap(flex_buffer& other)
		{
			buffer_.swap(other.buffer_);

			pptr_ = other.pptr_;

			gptr_ = other.gptr_;
		}

		size_type size() noexcept
		{
			return static_cast<size_type>(pptr_ - gptr_);
		}

		size_type size() const noexcept
		{
			return static_cast<size_type>(pptr_ - gptr_);
		}

		void normalize()
		{
			if (pptr_ == 0)
				return;

			std::size_t seg = size();

			std::memmove(buffer_.data(), wdata(), seg);

			pptr_ = seg;

			gptr_ = 0;
		}

		void ensure()
		{
			if (active() > water_line)
				return;

			buffer_.resize(max_size() * 3);
		}

		size_type max_size()
		{
			return buffer_.size();
		}

		size_type max_size() const
		{
			return buffer_.size();
		}

		pos_type pubseekoff(off_type off, std::ios_base::seekdir way, std::ios_base::openmode mode)
		{
			off_type new_off{};

			switch (way)
			{
			case std::ios::beg:
				{
					new_off = 0;
				}

				break;
			case std::ios::cur:
				{
					if (mode & std::ios::in)
					{
						new_off = pptr_;
					}
					else if (mode & std::ios::out)
					{
						new_off = gptr_;
					}
				}
				break;
			case std::ios::end:
				{
					if (mode & std::ios::in)
					{
						new_off = pptr_;
					}
					else if (mode & std::ios::out)
					{
						new_off = gptr_;
					}
				}
				break;
			default:
				break;
			}

			off += new_off;

			off < 0 ? off = 0 : 0;

			if (mode & std::ios::in)
			{
				off > pptr_ ? pptr_ = off = pptr_ : pptr_ = off;
			}
			else if (mode & std::ios::out)
			{
				off > pptr_ ? gptr_ = off = pptr_ : gptr_ = off;
			}

			return static_cast<pos_type>(off);
		}

		pos_type pubseekpos(pos_type pos, std::ios_base::openmode mode)
		{
			if (mode & std::ios::in)
			{
				auto end_pos = static_cast<int>(buffer_.size());

				pos > end_pos ? pptr_ = end_pos : 0;

				pos < 0 ? pos = 0 : (pos_type)0;

				pptr_ = static_cast<off_type>(pos);
			}

			if (mode & std::ios::out)
			{
				pos > pptr_ ? gptr_ = pptr_ : 0;

				pos < 0 ? pos = 0 : (pos_type)0;

				gptr_ = static_cast<off_type>(pos);
			}

			return static_cast<pos_type>(pos);
		}

		size_type sputn(const value_type* begin, size_type size)
		{
			if (size > active())
				return 0;

			for (size_type i = 0; i < size; ++i)
			{
				*rdata() = begin[i];

				commit(1);
			}

			return size;
		}

		size_type sgetn(value_type* begin, size_type size)
		{
			if (size > this->size())
				return 0;

			for (size_type i = 0; i < size; ++i)
			{
				begin[i] = *wdata();

				consume(1);
			}

			return size;
		}

	private:
		std::vector<value_type, allocator_type> buffer_;

		off_type pptr_;

		off_type gptr_;
	};

	using flex_buffer_t = flex_buffer<uint8_t, std::char_traits<uint8_t>>;
} // namespace aquarius