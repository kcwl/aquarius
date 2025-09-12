#pragma once
#include <vector>
#include <expected>
#include <string>

namespace aquarius
{
	namespace detail
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

			flex_buffer(flex_buffer&& other)
				: buffer_(std::move(other.buffer_))
				, wpos_(std::exchange(other.wpos_, 0))
				, rpos_(std::exchange(other.rpos_, 0))
			{

			}

			flex_buffer(const flex_buffer& other)
			{
				if (this != std::addressof(other))
				{
					buffer_ = other.buffer_;
					wpos_ = other.wpos_;
					rpos_ = other.rpos_;
				}
			}

		public:
			T* rdata()
			{
				return buffer_.data() + rpos_;
			}

			const T* rdata() const
			{
				return buffer_.data() + rpos_;
			}

			T* wdata()
			{
				return buffer_.data() + wpos_;
			}

			const T* wdata() const
			{
				return buffer_.data() + wpos_;
			}

			T* data()
			{
				return buffer_.data();
			}

			const T* data() const
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
				size < remain() ? wpos_ += size : 0;
			}

			void consume(std::size_t size)
			{
				size < active() ? rpos_ += size : 0;
			}

			T peek()
			{
				return buffer_[rpos_];
			}

			T get()
			{
				if (rpos_ >= capacity)
				{
					return T{ -1 };
				}

				return buffer_[rpos_++];
			}

			void put(T value)
			{
				constexpr auto size = sizeof(T);

				if (size < remain())
					return;

				std::memcpy(wdata(), &value, size);

				commit(size);
			}

			template<typename V>
			void append(flex_buffer<V>&& buffer)
			{
				if (buffer.active() > remain())
					return;

				auto buf = flex_buffer<T>(std::move(buffer));

				std::memcpy(wdata(), buf.rdata(), buf.active());

				commit(buf.active());
			}

			bool empty() const
			{
				return rpos_ == wpos_;
			}

			std::size_t tellg() const
			{
				return wpos_;
			}

		private:
			impl_type buffer_;

			std::size_t wpos_;

			std::size_t rpos_;
		};
	} // namespace detail
} // namespace aquarius