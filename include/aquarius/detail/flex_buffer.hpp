#pragma once
#include <aquarius/asio.hpp>
#include <aquarius/logger.hpp>

namespace aquarius
{
	namespace detail
	{
		template <typename Allocator = std::allocator<char>>
		class basic_flexbuf : public std::streambuf, private asio::noncopyable
		{
		public:
			using const_buffers_type = boost::asio::const_buffer;

			using mutable_buffers_type = boost::asio::mutable_buffer;

			constexpr static std::size_t buffer_delta = 256;

		public:
			basic_flexbuf(std::size_t max_size = std::numeric_limits<std::size_t>::max(), Allocator alloc = Allocator{})
				: max_size_(max_size)
				, buffer_(alloc)
			{
				reset(max_size);
			}

			virtual ~basic_flexbuf() = default;

		public:
			basic_flexbuf(basic_flexbuf&& other) noexcept
				: max_size_(std::exchange(other.max_size_, 0))
				, buffer_(std::move(other.buffer_))
			{
				setg(other.eback(), other.gptr(), other.egptr());
				setp(other.pptr(), other.epptr());

				// other.reset(buffer_delta);
			}

			basic_flexbuf& operator=(basic_flexbuf&& other) noexcept
			{
				if (this != std::addressof(other))
				{
					max_size_ = std::exchange(other.max_size_, std::numeric_limits<std::size_t>::max());
					buffer_ = std::move(other.buffer_);

					setg(other.eback(), other.gptr(), other.egptr());
					setp(other.pbase(), other.pptr());

					// other.reset(buffer_delta);
				}

				return *this;
			}

		public:
			std::size_t size() const
			{
				return pptr() - gptr();
			}

			std::size_t max_size() const
			{
				return max_size_;
			}

			std::size_t tellp() const
			{
				return pptr() - buffer_.data();
			}

			std::size_t tellg() const
			{
				return gptr() - buffer_.data();
			}

			std::size_t capacity() const
			{
				return buffer_.capacity();
			}

			const_buffers_type data() const
			{
				return boost::asio::buffer(boost::asio::const_buffer(gptr(), size() * sizeof(char_type)));
			}

			mutable_buffers_type prepare(std::size_t n)
			{
				reserve(n);

				return boost::asio::buffer(boost::asio::mutable_buffer(pptr(), n * sizeof(char_type)));
			}

			void commit(std::size_t n)
			{
				n = std::min<std::size_t>(n, epptr() - pptr());

				pbump(static_cast<int>(n));

				setg(eback(), gptr(), pptr());
			}

			void consume(std::size_t n)
			{
				if (egptr() < pptr())
				{
					setg(&buffer_[0], gptr(), pptr());
				}

				n = std::min<std::size_t>(n, size());

				gbump(static_cast<int>(n));
			}

			void reset(std::size_t n)
			{
				std::size_t pend = (std::min<std::size_t>)(n, buffer_delta);
				buffer_.resize(pend);
				setg(&buffer_[0], &buffer_[0], &buffer_[0]);
				setp(&buffer_[0], &buffer_[0] + pend);
			}

		protected:
			virtual int_type underflow() override
			{
				if (gptr() < pptr())
				{
					setg(&buffer_[0], gptr(), pptr());

					return traits_type::to_int_type(*gptr());
				}

				return traits_type::eof();
			}

			virtual int_type overflow(int_type c) override
			{
				if (traits_type::eq_int_type(c, traits_type::eof()))
					return traits_type::not_eof(c);

				auto buffer_size = size();

				bool result = true;

				if (pptr() == epptr())
				{
					buffer_size < max_size_&& max_size_ - buffer_size < buffer_delta ? result = reserve(max_size_ - buffer_size)
						: result = reserve(buffer_delta);
				}

				if (!result)
				{
					return traits_type::eof();
				}

				*pptr() = traits_type::to_char_type(c);

				pbump(1);

				return c;
			}

		private:
			bool reserve(std::size_t n)
			{
				std::size_t pnext = pptr() - &buffer_[0];
				std::size_t gnext = gptr() - &buffer_[0];
				std::size_t pend = epptr() - &buffer_[0];

				if (n <= pend - pnext)
					return false;

				if (gnext > 0)
				{
					pnext -= gnext;

					std::copy(&buffer_[0], &buffer_[0] + gnext, &buffer_[0] + gnext + pnext);
				}

				if (pend - pnext < n)
				{
					if (n <= max_size_ && pnext <= max_size_ - n)
					{
						pend = pnext + n;
						buffer_.resize(std::max<std::size_t>(pend, 1));
					}
					else
					{
						// max then max_size and not allocate space and not receive data
						XLOG_ERROR() << "buffer overflow! current size:" << size() << ", max_size:" << max_size()
							<< ", reserve size:" << n;
						return false;
					}
				}

				setg(&buffer_[0], &buffer_[0], &buffer_[0] + pnext);
				setp(&buffer_[0] + pnext, &buffer_[0] + pend);

				return true;
			}

			virtual pos_type seekoff(off_type offset, std::ios_base::seekdir dir,
									 std::ios_base::openmode mode = std::ios::in | std::ios::out) override
			{
				auto dist = pptr() - eback();

				int64_t new_off{};

				switch (dir)
				{
					case std::ios_base::beg:
						{
							new_off = 0;
						}
						break;
					case std::ios_base::cur:
						{
							auto both = std::ios_base::in | std::ios_base::out;

							if ((mode & both) != both)
							{
								if (mode & std::ios::in)
								{
									new_off = pptr() - eback();
								}
								else if (mode & std::ios::out)
								{
									new_off = gptr() - eback();
								}
							}
						}
						break;
					case std::ios_base::end:
						{
							new_off = dist;
						}
						break;
					default:
						break;
				}

				if (static_cast<unsigned long long>(offset) + new_off > static_cast<unsigned long long>(dist))
				{
					return pos_type{ off_type{-1} };
				}

				offset += new_off;

				if (static_cast<int64_t>(offset) < 0 )
				{
					return pos_type{ off_type{-1} };
				}

				if ((mode & std::ios::in) == std::ios::in)
				{
					setg(&buffer_[0], &buffer_[0] + offset, pptr());
				}

				if ((mode & std::ios::out) == std::ios::out)
				{
					setp(&buffer_[0] + offset, pptr());
				}

				return offset;
			}

			virtual pos_type seekpos(pos_type pos,
									 std::ios_base::openmode mode = std::ios::in | std::ios::out) override
			{
				if (mode & std::ios::in)
				{
					auto dist = static_cast<off_type>(pptr() - eback());

					if (pos > dist)
					{
						return pos_type(off_type(-1));
					}

					setg(&buffer_[0], &buffer_[0] + pos, pptr());
				}


				if (mode & std::ios::out)
				{
					auto dist = static_cast<off_type>(epptr() - eback());

					if (pos > dist)
					{
						return pos_type(off_type(-1));
					}

					setp(&buffer_[0] + pos, epptr());
				}

				return pos_type{pos};
			}

		private:
			std::size_t max_size_;

			std::vector<char_type, Allocator> buffer_;
		};

		template <typename Allocator>
		class basic_flexbuf_ref
		{
		public:
			typedef typename basic_flexbuf<Allocator>::const_buffers_type const_buffers_type;

			typedef typename basic_flexbuf<Allocator>::mutable_buffers_type mutable_buffers_type;

			explicit basic_flexbuf_ref(basic_flexbuf<Allocator>& sb)
				: sb_(sb)
			{}

			basic_flexbuf_ref(const basic_flexbuf_ref& other) noexcept
				: sb_(other.sb_)
			{}

			basic_flexbuf_ref(basic_flexbuf_ref&& other) noexcept
				: sb_(other.sb_)
			{}

			std::size_t size() const noexcept
			{
				return sb_.size();
			}

			std::size_t max_size() const noexcept
			{
				return sb_.max_size();
			}

			std::size_t capacity() const noexcept
			{
				return sb_.capacity();
			}

			const_buffers_type data() const noexcept
			{
				return sb_.data();
			}

			mutable_buffers_type prepare(std::size_t n)
			{
				return sb_.prepare(n);
			}

			void commit(std::size_t n)
			{
				return sb_.commit(n);
			}

			void consume(std::size_t n)
			{
				return sb_.consume(n);
			}

		private:
			basic_flexbuf<Allocator>& sb_;
		};
	} // namespace detail

	using flex_buffer = detail::basic_flexbuf<>;

	using flex_buffer_ref = detail::basic_flexbuf_ref<std::allocator<char>>;
} // namespace aquarius