#pragma once
#ifdef AQUARIUS_HAS_ASIO_STREAMBUF
#include <boost/asio/streambuf.hpp>
#else
#include <vector>
#endif

namespace aquarius
{
	namespace detail
	{
		template <typename _Elem, typename _Traits, typename _Alloc = std::allocator<_Elem>>
		class flex_buffer
		{
			using elem_type = _Elem;

			using allocator_type = _Alloc;

			using traits_type = _Traits;

			using this_type = flex_buffer<elem_type, traits_type, allocator_type>;

			constexpr static std::size_t water_line = 32;

		public:
			constexpr static std::size_t capacity = 512;

		public:
			using iterator = typename std::vector<elem_type, allocator_type>::iterator;
			using const_iterator = typename std::vector<elem_type, allocator_type>::const_iterator;
			using value_type = typename std::vector<elem_type, allocator_type>::value_type;
			using size_type = typename std::vector<elem_type, allocator_type>::size_type;
			using reference = typename std::vector<elem_type, allocator_type>::reference;
			using const_reference = typename std::vector<elem_type, allocator_type>::const_reference;
			using pointer = typename std::vector<elem_type, allocator_type>::pointer;
			using const_pointer = typename std::vector<elem_type, allocator_type>::const_pointer;

			using seekdir = std::ios::seekdir;
			using openmode = std::ios::openmode;

			using pos_type = typename traits_type::pos_type;
			using off_type = typename traits_type::off_type;

		public:
			flex_buffer(const std::size_t capa = capacity)
				: buffer_(capa, 0)
				, pptr_(0)
				, gptr_(0)
				, pcount_(capa)
				, start_(0)
				, has_success_(true)
			{
			}

			flex_buffer(flex_buffer&& other)
				: buffer_(std::move(other.buffer_))
				, pptr_(std::exchange(other.pptr_, 0))
				, gptr_(std::exchange(other.gptr_, 0))
				, pcount_(std::exchange(other.pcount_, 0))
				, start_(std::exchange(other.start_, 0))
				, has_success_(std::exchange(other.has_success_, true))
			{
			}

			flex_buffer(const flex_buffer&) = default;
			flex_buffer& operator=(const flex_buffer&) = default;

			flex_buffer& operator=(flex_buffer&& other) noexcept
			{
				if (this != std::addressof(other))
				{
					buffer_ = std::move(other.buffer_);
					pptr_ = std::exchange(other.pptr_, 0);
					gptr_ = std::exchange(other.gptr_, 0);
					pcount_ = std::exchange(other.pcount_, 0);
					start_ = std::exchange(other.start_, 0);
					has_success_ = std::exchange(other.has_success_, true);
				}

				return *this;
			}

			virtual ~flex_buffer() = default;

		public:
			const size_type active() const noexcept
			{
				return static_cast<size_type>(pcount_);
			}

			pointer wdata()
			{
				return buffer_.data() + static_cast<size_type>(gptr_);
			}

			const_pointer wdata() const noexcept
			{
				return buffer_.data() + static_cast<size_type>(gptr_);
			}

			pointer rdata()
			{
				return buffer_.data() + static_cast<size_type>(pptr_);
			}

			const_pointer rdata() const noexcept
			{
				return buffer_.data() + static_cast<size_type>(pptr_);
			}

			void commit(off_type bytes)
			{
				pptr_ += bytes;
				pcount_ -= bytes;
			}

			void consume(off_type bytes)
			{
				gptr_ += bytes;
			}

			size_type size() const noexcept
			{
				return static_cast<std::size_t>(pptr_ - gptr_);
			}

			void normalize()
			{
				if (pptr_ == 0)
					return;

				traits_type::copy(buffer_.data(), wdata(), size());

				pptr_ -= gptr_;
				pcount_ += gptr_;
				gptr_ = 0;
			}

			void ensure()
			{
				if (active() > water_line)
					return;

				buffer_.resize(buffer_.size() + capacity);

				pcount_ += capacity;
			}

			bool save(const value_type* data, const size_type size)
			{
				if (pcount_ < static_cast<off_type>(size))
				{
					size_type new_capacity = capacity;

					new_capacity < size ? new_capacity = size : 0;

					buffer_.resize(new_capacity + buffer_.size());

					pcount_ += new_capacity;
				}

				traits_type::copy(rdata(), data, size);

				commit(size);

				return true;
			}

			bool load(value_type* data, const size_type new_size)
			{
				if (size() < new_size)
					return false;

				traits_type::copy(data, wdata(), new_size);

				consume(new_size);

				return true;
			}

			pos_type pubseekoff(off_type off, seekdir dir, openmode mode)
			{
				pos_type new_off{};

				switch (dir)
				{
					case std::ios::beg:
					{
						new_off = 0;
					}
					break;
					case std::ios::cur:
					{
						if (mode == std::ios::in)
						{
							new_off = pptr_;
						}
						else if (mode == std::ios::out)
						{
							new_off = gptr_;
						}
					}
					break;
					case std::ios::end:
					{
						new_off = static_cast<pos_type>(buffer_.size());
					}
					break;
					default:
					return -1;
				}

				off += static_cast<off_type>(new_off);

				if (off > static_cast<off_type>(buffer_.size()) && mode == std::ios::in)
					return -1;

				if (off > pptr_ && mode == std::ios::out)
					return -1;

				if (mode == std::ios::in)
				{
					pptr_ = off;
				}

				if (mode == std::ios::out)
				{
					gptr_ = static_cast<pos_type>(off);
				}

				return off;
			}

			pos_type pubseekpos(pos_type pos, openmode mode)
			{
				switch (mode)
				{
					case std::ios::in:
					{
						if (pos > static_cast<pos_type>(buffer_.size()))
							pos = static_cast<pos_type>(buffer_.size() - 1);

						pptr_ = pos;
					}
					break;
					case std::ios::out:
					{
						if (pos > pptr_)
							return -1;

						gptr_ = pos;
					}
					break;
					default:
					pos = -1;
					break;
				}

				return pos;
			}

			bool start()
			{
				if (start_ != 0)
					return false;

				start_ = pubseekoff(0, std::ios::cur, std::ios::out);

				return true;
			}

			void close()
			{
				if (has_success_)
					return;

				pubseekpos(start_, std::ios::out);

				start_ = 0;
			}

			void failed()
			{
				has_success_ = false;
			}

			bool success() const
			{
				return has_success_;
			}

		private:
			std::vector<value_type, allocator_type> buffer_;

			pos_type pptr_;

			pos_type gptr_;

			off_type pcount_;

			pos_type start_;

			bool has_success_;
		};
	}

#ifdef AQUARIUS_HAS_ASIO_STREAMBUF
	using flex_buffer_t = boost::asio::streambuf;
#else
	using flex_buffer_t = detail::flex_buffer<uint8_t, std::char_traits<uint8_t>>;
#endif
}