#pragma once
#include <aquarius/asio.hpp>
#include <aquarius/logger.hpp>

namespace aquarius
{
	template <typename Allocator = std::allocator<char>>
	class basic_flexbuf : public std::streambuf, private noncopyable
	{
	public:
		using const_buffers_type = boost::asio::const_buffer;

		using mutable_buffers_type = boost::asio::mutable_buffer;

		constexpr static std::size_t buffer_delta = 256;

	public:
		basic_flexbuf(std::size_t max_size = std::numeric_limits<std::size_t>::max(),
							   Allocator alloc = Allocator{})
			: max_size_(max_size)
			, buffer_(alloc)
		{
			std::size_t pend = (std::min<std::size_t>)(max_size, buffer_delta);
			buffer_.resize(pend);
			setg(&buffer_[0], &buffer_[0], &buffer_[0]);
			setp(&buffer_[0], &buffer_[0] + pend);
		}

		virtual ~basic_flexbuf() = default;

	public:
		basic_flexbuf(basic_flexbuf&& other) noexcept
			: max_size_(std::exchange(other.max_size_, 0))
			, buffer_(std::move(other.buffer_))
		{
			setg(other.eback(), other.gptr(), other.egptr());
			setp(other.pptr(), other.epptr());
		}

		basic_flexbuf& operator=(basic_flexbuf&& other) noexcept
		{
			if (this != std::addressof(other))
			{
				max_size_ = std::exchange(other.max_size_, 0);
				buffer_ = std::move(other.buffer_);

				setg(other.eback(), other.gptr(), other.egptr());
				setp(other.pbase(), other.pptr());
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

			if (pptr() == epptr())
			{
				buffer_size < max_size_&& max_size_ - buffer_size < buffer_delta ? reserve(max_size_ - buffer_size)
																				 : reserve(buffer_delta);
			}

			*pptr() = traits_type::to_char_type(c);

			pbump(1);

			return c;
		}

	private:
		void reserve(std::size_t n)
		{
			std::size_t pnext = pptr() - &buffer_[0];
			std::size_t gnext = gptr() - &buffer_[0];
			std::size_t pend = epptr() - &buffer_[0];

			if (n <= pend - pnext)
				return;

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
				}
			}

			setg(&buffer_[0], &buffer_[0], &buffer_[0] + pnext);
			setp(&buffer_[0] + pnext, &buffer_[0] + pend);
		}

		virtual pos_type seekoff(off_type offset, std::ios_base::seekdir dir,
								 std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out) override
		{
			auto pos = static_cast<int32_t>(calc_pos(mode));
			switch (dir)
			{
			case std::ios_base::beg:
				{
					offset = std::min<off_type>(offset, pos);
				}
				break;
			case std::ios_base::cur:
			case std::ios_base::end:
				{
					offset = std::min<off_type>(pos + offset, pos);
				}
				break;
			default:
				break;
			}

			set_pos(offset, mode);

			return offset;
		}

		std::size_t calc_pos(std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out)
		{
			switch (mode)
			{
			case std::ios_base::in:
				{
					return pptr() - (&buffer_[0]);
				}
				break;
			case std::ios_base::out:
				{
					return gptr() - (&buffer_[0]);
				}
				break;
			default:
				break;
			}

			return static_cast<std::size_t>(-1);
		}

		void set_pos(off_type offset, std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out)
		{
			switch (mode)
			{
			case std::ios_base::in:
				{
					setp(&buffer_[0] + offset, epptr());
					setg(&buffer_[0], gptr(), pptr());
				}
				break;
			case std::ios_base::out:
				{
					setg(&buffer_[0], &buffer_[0] + offset, pptr());
				}
				break;
			default:
				break;
			}
		}

		virtual pos_type seekpos(pos_type pos,
								 std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out) override
		{
			int32_t offset = static_cast<std::size_t>(pos) < calc_pos(mode) ? static_cast<int>(pos) : 0;

			set_pos(offset, mode);

			return offset;
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
		{
		}

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

	using flex_buffer = basic_flexbuf<>;

	using flex_buffer_ref = basic_flexbuf_ref<std::allocator<char>>;

	class auto_consume
	{
	public:
		auto_consume(flex_buffer& buffer)
			: buffer_(buffer)
		{}

		~auto_consume()
		{
			buffer_.consume(1);
		}

	private:
		flex_buffer& buffer_;
	};

	class auto_commit
	{
	public:
		auto_commit(flex_buffer& buffer)
			: buffer_(buffer)
		{}

		~auto_commit()
		{
			buffer_.consume(1);
		}

	private:
		flex_buffer& buffer_;
	};
} // namespace aquarius