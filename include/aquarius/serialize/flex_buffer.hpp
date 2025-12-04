#pragma once
#include <vector>
#include <expected>
#include <string>
#include <aquarius/serialize/error.hpp>

namespace aquarius
{
	class flex_buffer
	{
	public:
		constexpr static std::size_t capacity = 8192;

		constexpr static std::size_t npos = static_cast<std::size_t>(-1);

		using value_type = uint8_t;

		using impl_type = std::vector<value_type>;

		using size_type = std::size_t;

		using pos_type = std::size_t;

	public:
		flex_buffer()
			: buffer_(capacity)
			, wpos_(0)
			, rpos_(0)
		{}

		flex_buffer(value_type* data, std::size_t len)
			: buffer_(len)
			, wpos_(0)
			, rpos_(0)
		{
			put(data, data + len);
		}

		flex_buffer(flex_buffer&& other) noexcept
			: buffer_(std::move(other.buffer_))
			, wpos_(std::exchange(other.wpos_, 0))
			, rpos_(std::exchange(other.rpos_, 0))
		{}

		flex_buffer& operator=(flex_buffer&& other) noexcept
		{
			if (this != std::addressof(other))
			{
				buffer_ = std::move(other.buffer_);
				wpos_ = std::exchange(other.wpos_, 0);
				rpos_ = std::exchange(other.rpos_, 0);
			}

			return *this;
		}

		flex_buffer(const flex_buffer& other) = delete;
		flex_buffer& operator=(const flex_buffer& other) = delete;

		virtual ~flex_buffer() = default;

	public:
		value_type* rdata()
		{
			return buffer_.data() + rpos_;
		}
		const value_type* rdata() const
		{
			return buffer_.data() + rpos_;
		}

		value_type* wdata()
		{
			return buffer_.data() + wpos_;
		}

		value_type* data()
		{
			return buffer_.data();
		}

		const value_type* data() const
		{
			return buffer_.data();
		}

		size_type active() const
		{
			return wpos_ - rpos_;
		}

		std::size_t remain() const
		{
			return buffer_.size() - wpos_;
		}

		void commit(size_type size)
		{
			size <= remain() ? wpos_ += size : 0;
		}

		void consume(size_type size)
		{
			size <= active() ? rpos_ += size : 0;
		}

		value_type peek()
		{
			return buffer_[rpos_];
		}

		value_type get(error_code& ec)
		{
			if (rpos_ >= capacity)
			{
				ec = serialize_error::overflow;
				return {};
			}

			return buffer_[rpos_++];
		}

		value_type get()
		{
			error_code ec;

			value_type value = get(ec);

			if (ec)
				throw ec;

			return value;
		}

		template <typename Pod>
		requires(std::is_trivial_v<Pod> && std::is_standard_layout_v<Pod>)
		Pod get(error_code& ec)
		{
			Pod value{};

			constexpr auto size = sizeof(Pod);

			if (active() < size)
			{
				ec = serialize_error::not_enough_space;

				return value;
			}

			std::copy(rdata(), rdata() + size, (char*)&value);

			consume(size);

			ec = serialize_error::success;

			return value;
		}

		template <typename Pod>
		requires(std::is_trivial_v<Pod> && std::is_standard_layout_v<Pod>)
		Pod get()
		{
			error_code ec;

			Pod value = get<Pod>(ec);

			if (ec)
				throw ec;

			return value;
		}

		void get(std::string& json)
		{
			json = std::string((char*)rdata(), active());

			consume(active());
		}

		error_code put(value_type value, error_code& ec)
		{
			ec = serialize_error::success;

			constexpr auto size = sizeof(value_type);

			if (size > remain())
			{
				return ec = serialize_error::not_enough_space;
			}

			std::copy(&value, (value_type*)&value + size, wdata());

			commit(size);

			return ec;
		}

		void put(value_type value)
		{
			error_code ec{};

			put(value, ec);

			if (ec)
				throw ec;
		}

		template <typename Iterator>
		error_code put(Iterator begin, Iterator end, error_code& ec)
		{
			std::size_t size = std::distance(begin, end);

			if (size > remain())
			{
				return ec = serialize_error::not_enough_space;
			}

			std::copy(begin, end, wdata());

			commit(size);

			return ec = serialize_error::success;
		}

		template <typename Iterator>
		void put(Iterator begin, Iterator end)
		{
			error_code ec{};
			put(begin, end, ec);

			if (ec)
				throw ec;
		}

		error_code put(value_type* data, std::size_t len, error_code& ec)
		{
			if (len > remain())
			{
				return ec = serialize_error::not_enough_space;
			}

			return put(data, data + len, ec);
		}

		void put(value_type* data, std::size_t len)
		{
			error_code ec{};

			put(data, len, ec);

			if (ec)
				throw ec;
		}

		error_code append(flex_buffer&& buffer, error_code& ec)
		{
			if (buffer.active() > remain())
			{
				return ec = serialize_error::not_enough_space;
			}

			auto buf = flex_buffer(std::move(buffer));

			std::copy(buf.rdata(), buf.rdata() + buf.active(), wdata());

			commit(buf.active());

			return ec = serialize_error::success;
		}

		error_code append(flex_buffer&& buffer)
		{
			error_code ec{};

			append(std::move(buffer), ec);

			if (ec)
				throw ec;

			return ec;
		}

		bool empty() const
		{
			return rpos_ == wpos_;
		}

		std::size_t tellp() const
		{
			return rpos_;
		}

		std::size_t tellg() const
		{
			return wpos_;
		}

		void swap(flex_buffer& fb)
		{
			std::swap(buffer_, fb.buffer_);
			std::swap(wpos_, fb.wpos_);
			std::swap(rpos_, fb.rpos_);
		}

		template<char... args>
		std::string get_first_range()
		{
			auto iter = std::find_if(buffer_.begin() + rpos_, buffer_.end(), [&] (const auto c) { return ((c == args) || ...); });

			if (iter == buffer_.end())
			{
				return std::string((char*)rdata(), active());
			}

			auto len = std::distance(buffer_.begin() + rpos_, iter);

			auto value = std::string((char*)rdata(), len);

			consume(len + 1);

			return value;
		}

	private:
		impl_type buffer_;

		std::size_t wpos_;

		std::size_t rpos_;
	};
} // namespace aquarius