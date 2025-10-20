#pragma once
#include <vector>
#include <expected>
#include <string>
#include <aquarius/serialize/error.hpp>

namespace aquarius
{
	template <typename T>
	class flex_buffer
	{
		constexpr static std::size_t capacity = 8192;

		using impl_type = std::vector<T>;

	public:
		flex_buffer()
			: buffer_(capacity)
			, wpos_(0)
			, rpos_(0)
		{}

		flex_buffer(T* data, std::size_t len)
			: buffer_(len)
			, wpos_(0)
			, rpos_(0)
		{
			std::copy(data, data + len, wdata());
			commit(len);
		}

		flex_buffer(flex_buffer&& other) noexcept
			: buffer_(std::move(other.buffer_))
			, wpos_(std::exchange(other.wpos_, 0))
			, rpos_(std::exchange(other.rpos_, 0))
		{
			other.buffer_.resize(capacity);
		}

		flex_buffer(const flex_buffer& other) noexcept
		{
			if (this != std::addressof(other))
			{
				buffer_ = other.buffer_;
				wpos_ = other.wpos_;
				rpos_ = other.rpos_;
			}
		}

		virtual ~flex_buffer() = default;

	public:
		T* rdata()
		{
			return buffer_.data() + rpos_;
		}

		T* wdata()
		{
			return buffer_.data() + wpos_;
		}

		T* data()
		{
			return buffer_.data();
		}

		std::size_t active() const
		{
			return wpos_ - rpos_;
		}

		std::size_t remain() const
		{
			return buffer_.size() - wpos_;
		}

		void commit(std::size_t size)
		{
			size <= remain() ? wpos_ += size : 0;
		}

		void consume(std::size_t size)
		{
			size <= active() ? rpos_ += size : 0;
		}

		T peek()
		{
			return buffer_[rpos_];
		}

		T get(error_code& ec)
		{
			if (rpos_ >= capacity)
			{
				ec = serialize_error::overflow;
				return {};
			}

			return buffer_[rpos_++];
		}

		T get()
		{
			error_code ec;

			T value = get(ec);

			if(ec)
				throw ec;

			return value;
		}

		template<typename Pod>
		requires(std::is_trivial_v<Pod>&& std::is_standard_layout_v<Pod>)
		Pod get(error_code& ec)
		{
			Pod value{};

			constexpr auto size = sizeof(Pod);

			if (active() < size)
			{
				ec = serialize_error::not_enough_space;

				return value;
			}

			std::memcpy((char*)&value, rdata(), size);

			consume(size);

			ec = serialize_error::success;

			return value;
		}

		template<typename Pod>
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
			json = std::string(rdata(), active());

			consume(json.size());
		}

		error_code put(T value, error_code& ec)
		{
			ec = serialize_error::success;

			constexpr auto size = sizeof(T);

			if (size > remain())
			{
				return ec = serialize_error::not_enough_space;
			}

			std::memcpy(wdata(), &value, size);

			commit(size);

			return ec;
		}

		void put(T value)
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

		error_code put(T* data, std::size_t len, error_code& ec)
		{
			if (len > remain())
			{
				return ec = serialize_error::not_enough_space;
			}

			return put(data, data + len, ec);
		}

		void put(T* data, std::size_t len)
		{
			error_code ec{};

			put(data, len, ec);

			if (ec)
				throw ec;
		}

		bool empty() const
		{
			return rpos_ == wpos_;
		}

		std::size_t tellg() const
		{
			return wpos_;
		}

		template <typename V>
		error_code append(flex_buffer<V>&& buffer, error_code& ec)
		{
			if (buffer.active() > remain())
			{
				return ec = serialize_error::not_enough_space;
			}

			auto buf = flex_buffer<T>(std::move(buffer));

			std::memcpy(wdata(), buf.rdata(), buf.active());

			commit(buf.active());

			return ec = serialize_error::success;
		}

		template <typename V>
		error_code append(flex_buffer<V>&& buffer)
		{
			error_code ec{};
			
			append(std::move(buffer), ec);

			if (ec)
				throw ec;

			return ec;
		}

	private:
		impl_type buffer_;

		std::size_t wpos_;

		std::size_t rpos_;
	};
} // namespace aquarius