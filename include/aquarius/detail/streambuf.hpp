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
			}

			template <typename _Ty, std::size_t N, typename = std::is_convertible<_Ty, _Elem>>
			basic_streambuf(std::span<_Ty, N> data)
				: basic_streambuf()
			{}

			basic_streambuf(basic_streambuf&& other)
			{
				if (this != std::addressof(other))
				{
					this->swap(other);
				}
			}

			basic_streambuf(const void* buffer, std::size_t sz)
				: basic_streambuf()
			{
				if (sz < active())
				{
					std::memcpy(wdata(), buffer, sz);
					commit(sz);
				}
			}

			basic_streambuf(const basic_streambuf&) = delete;
			basic_streambuf& operator=(const basic_streambuf&) = delete;

		public:
			std::size_t active() noexcept
			{
				return base_type::epptr() - base_type::gptr();
			}

			std::size_t active() const noexcept
			{
				return base_type::epptr() - base_type::gptr();
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
				return base_type::pptr();
			}

			const_pointer wdata() const noexcept
			{
				return base_type::pptr();
			}

			pointer rdata() noexcept
			{
				return base_type::gptr();
			}

			const_pointer rdata() const noexcept
			{
				return base_type::gptr();
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
				buffer_.resize(bytes);
			}

			void swap(basic_streambuf& buf)
			{
				base_type::swap(buf);

				buffer_.swap(buf.buffer_);
			}

			auto erase(const_iterator& where)
			{
				this->consume(-1);

				auto iter = buffer_.erase(where);

				reset();

				return iter;
			}

			auto erase(const_iterator& begin, const_iterator& end)
			{
				auto dis = std::distance(begin, end);

				this->consume(-dis);

				auto iter = buffer_.erase(begin, end);

				reset();

				return iter;
			}

			base_type* rdbuf()
			{
				return static_cast<base_type*>(this);
			}

			std::size_t size() noexcept
			{
				return base_type::pptr() - rdata();
			}

			std::size_t size() const noexcept
			{
				return base_type::pptr() - rdata();
			}

			void normalize()
			{
				if (rdata() == &buffer_[0])
					return;

				std::memmove(buffer_.data(), rdata(), wdata() - rdata());

				reset();
			}

			void ensure()
			{
				if (active() != 0)
					return;

				buffer_.resize(buffer_.size() * 2);
			}

		protected:
			virtual int_type overflow(int_type meta = _Traits::eof()) override
			{
				if (_Traits::eq_int_type(meta, _Traits::eof()))
					return _Traits::not_eof(meta);

				const auto pptr = base_type::pptr();
				const auto epptr = base_type::epptr();

				if (epptr - pptr < meta)
					return _Traits::eof();

				base_type::setg(base_type::eback(), base_type::gptr(), base_type::pptr());

				return meta;
			}

			virtual int_type underflow() override
			{
				const auto gptr = base_type::gptr();

				if (!gptr)
					return _Traits::eof();

				if (gptr < base_type::egptr())
					return _Traits::eof();

				const auto pptr = base_type::pptr();

				if (!pptr)
					return _Traits::eof();

				if (pptr == base_type::eback())
					return _Traits::eof();

				if (gptr == pptr)
					return _Traits::eof();

				base_type::setg(&buffer_[0], base_type::gptr(), base_type::pptr());

				return _Traits::to_int_type(*base_type::gptr());
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

				if (off != 0 && (((mode & std::ios::in) && !gptr_old) || (mode & std::ios::out) && !pptr_old))
					return pos_type(off_type(-1));

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

				if (off != 0 && (((mode & std::ios::in) && !gptr_old) || ((mode & std::ios::out) && !pptr_old)))
					return pos_type(off_type(-1));

				const auto new_ptr = seek_low + off;

				if ((mode & std::ios::in) && gptr_old)
				{
					base_type::setg(seek_low, new_ptr, pptr_old);
				}

				if ((mode & std::ios::out) && pptr_old)
				{
					base_type::setp(seek_low, new_ptr, base_type::epptr());
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