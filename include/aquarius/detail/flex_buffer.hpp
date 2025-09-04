#pragma once
#include <vector>
#include <expected>
#include <string>
#include <span>

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

			T peek()
			{
				return buffer_[rpos_];
			}

			T get()
			{
				T result = peek();

				rpos_++;

				return result;
			}

		private:
			impl_type buffer_;

			std::size_t wpos_;

			std::size_t rpos_;
		};
	} // namespace detail
} // namespace aquarius