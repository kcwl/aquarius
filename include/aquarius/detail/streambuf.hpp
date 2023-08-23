#pragma once
#include <ios>
#include <iterator>
#include <span>
#include <streambuf>
#include <vector>

namespace aquarius
{
	namespace detail
	{
		constexpr static std::size_t capacity = 4096;

		constexpr static std::size_t water_line = 32;

		template <typename _Elem, typename _Traits, typename _Alloc = std::allocator<_Elem>>
		class basic_streambuf : public std::basic_streambuf<_Elem, _Traits>
		{
			using allocator_type = _Alloc;

			using base_type = std::basic_streambuf<_Elem, _Traits>;

		public:
			using iterator = typename std::vector<_Elem, allocator_type>::iterator;
			using const_iterator = typename std::vector<_Elem, allocator_type>::const_iterator;
			using value_type = typename std::vector<_Elem, allocator_type>::value_type;
			using size_type = typename std::vector<_Elem, allocator_type>::size_type;
			using reference = typename std::vector<_Elem, allocator_type>::reference;
			using const_reference = typename std::vector<_Elem, allocator_type>::const_reference;
			using pointer = typename std::vector<_Elem, allocator_type>::pointer;
			using const_pointer = std::vector<_Elem, allocator_type>::const_pointer;

			using off_type = typename base_type::off_type;
			using pos_type = typename base_type::pos_type;
			using int_type = typename _Traits::int_type;

		public:
			basic_streambuf()
				: basic_streambuf(capacity)
			{}

			basic_streambuf(size_type number)
				: buffer_(number)
			{
				reset();
			}

			template <typename _Iter>
			basic_streambuf(_Iter begin, _Iter end)
				: basic_streambuf()
			{
				buffer_.clear();

				std::copy(begin, end, std::back_inserter(buffer_));

				commit(static_cast<int>(buffer_.size()));
			}

			template <typename _Ty, std::size_t N, typename = std::is_convertible<_Ty, _Elem>>
			basic_streambuf(std::span<_Ty, N> data)
				: basic_streambuf(data.begin(), data.end())
			{
				static_assert(N <= capacity, "maybe input is too large!");
			}

			basic_streambuf(basic_streambuf&& other)
			{
				if (this != std::addressof(other))
				{
					this->swap(other);
				}
			}

			basic_streambuf(const void* buffer, size_type sz)
				: basic_streambuf()
			{
				if (sz < active())
				{
					std::memcpy(rdata(), buffer, sz);
					commit(static_cast<int>(sz));
				}
			}

			basic_streambuf(const basic_streambuf&) = delete;
			basic_streambuf& operator=(const basic_streambuf&) = delete;

			bool operator==(const basic_streambuf& other) const
			{
				return buffer_ == other.buffer_;
			}

		public:
			size_type active() noexcept
			{
				return base_type::epptr() - base_type::pptr();
			}

			size_type active() const noexcept
			{
				return base_type::epptr() - base_type::pptr();
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
				return base_type::gptr();
			}

			const_pointer wdata() const noexcept
			{
				return base_type::gptr();
			}

			pointer rdata() noexcept
			{
				return base_type::pptr();
			}

			const_pointer rdata() const noexcept
			{
				return base_type::pptr();
			}

			void commit(int bytes)
			{
				bytes = static_cast<int>((std::min<std::size_t>)(bytes, base_type::epptr() - base_type::pptr()));
				base_type::pbump(static_cast<int>(bytes));
				base_type::setg(base_type::eback(), base_type::gptr(), base_type::pptr());
			}

			void consume(int bytes)
			{
				if (base_type::egptr() < base_type::pptr())
					base_type::setg(&buffer_[0], base_type::gptr(), base_type::pptr());

				if (base_type::gptr() + bytes > base_type::pptr())
					bytes = static_cast<int>(base_type::pptr() - base_type::gptr());

				if (base_type::gptr() + bytes < base_type::eback())
					bytes = static_cast<int>(base_type::eback() - base_type::gptr());

				base_type::gbump(bytes);
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

				reset();
			}

			void resize(size_type bytes)
			{
				bytes == 0 ? bytes = capacity : 0;

				auto old_size = buffer_.size();

				if (bytes <= old_size)
					return;

				buffer_.resize(bytes);

				reset();
			}

			void swap(basic_streambuf& buf)
			{
				base_type::swap(buf);

				buffer_.swap(buf.buffer_);
			}

			size_type size() noexcept
			{
				return base_type::pptr() - wdata();
			}

			size_type size() const noexcept
			{
				return base_type::pptr() - wdata();
			}

			void normalize()
			{
				if (rdata() == &buffer_[0])
					return;

				int seg = static_cast<int>(rdata() - wdata());

				seg <= 0 ? seg = 0 : 0;

				std::memmove(buffer_.data(), wdata(), seg);
					
				reset();

				commit(seg);
			}

			void ensure()
			{
				if (active() > water_line)
					return;

				buffer_.resize(max_size() * 2);
			}

			size_type max_size()
			{
				return buffer_.size();
			}

			size_type max_size() const
			{
				return buffer_.size();
			}

		protected:
			virtual int_type underflow() override
			{
				const auto pptr = base_type::pptr();

				if (pptr == base_type::eback())
					return _Traits::eof();

				auto result = _Traits::to_int_type(*base_type::gptr());

				consume(1);

				return result;
			}

			virtual pos_type seekoff(off_type off, std::ios_base::seekdir way, std::ios_base::openmode mode) override
			{
				// change position by offset, according to way and mode

				const auto pptr_old = base_type::pptr();

				const auto gptr_old = base_type::gptr();

				const auto seek_low = base_type::eback();

				const auto seek_dist = pptr_old - seek_low;

				off_type new_off{};

				switch (way)
				{
				case std::ios::beg:
					{
						new_off = 0;
					}
					break;
				case std::ios::end:
					{
						new_off = pptr_old - base_type::eback();
					}
					break;
				case std::ios::cur:
					{
						constexpr auto both = std::ios::in | std::ios::out;
						if ((mode & both) != both)
						{
							if (mode & std::ios::in)
							{
								if (gptr_old || !base_type::eback())
								{
									new_off = gptr_old - base_type::eback();
								}
							}
							else if ((mode & std::ios::out) && (pptr_old || !base_type::eback()))
							{
								new_off = pptr_old - base_type::eback();
							}
						}
					}
					break;
				default:
					return pos_type(off_type(-1));
				}

				if (static_cast<unsigned long long>(off) + new_off > static_cast<unsigned long long>(seek_dist))
					return pos_type(off_type(-1));

				off += new_off;

				const auto new_ptr = seek_low + off;

				if ((mode & std::ios::in) && gptr_old)
					base_type::setg(seek_low, new_ptr, pptr_old);

				if ((mode & std::ios::out) && pptr_old)
					base_type::setg(seek_low, new_ptr, base_type::epptr());

				return pos_type(off);
			}

			virtual pos_type seekpos(pos_type pos, std::ios_base::openmode mode) override
			{
				// change to specified position, according to mode
				const auto off = static_cast<std::streamoff>(pos);

				const auto gptr_old = base_type::gptr();

				const auto pptr_old = base_type::pptr();

				const auto seek_low = base_type::eback();

				const auto seek_dist = pptr_old - seek_low;

				if (static_cast<unsigned long long>(off) > static_cast<unsigned long long>(seek_dist))
					return pos_type(off_type(-1));

				const auto new_ptr = seek_low + off;

				if ((mode & std::ios::in) && gptr_old)
				{
					base_type::setg(seek_low, new_ptr, pptr_old);
				}

				if ((mode & std::ios::out) && pptr_old)
				{
					base_type::setp(seek_low, new_ptr);
				}

				return pos_type(off);
			}

		private:
			void reset()
			{
				if (buffer_.empty())
					return;

				base_type::setg(&buffer_[0], &buffer_[0], &buffer_[0]);
				base_type::setp(&buffer_[0], &buffer_[0] + buffer_.size());
			}

		private:
			std::vector<_Elem, allocator_type> buffer_;
		};
	} // namespace detail
} // namespace aquarius