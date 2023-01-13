#pragma once
#include <aquarius/impl/field.hpp>
#include <aquarius/impl/flex_buffer.hpp>

namespace aquarius
{
	namespace impl
	{
		template <typename _Ty>
		class header_of : public fields<_Ty>
		{
		public:
			using value_type = _Ty;

		public:
			header_of()
			{
				this->alloc(header_ptr_);
			}

			header_of(const header_of&) = default;

			header_of(header_of&&) = default;

			header_of& operator=(const header_of&) = default;

			~header_of()
			{
				this->dealloc(header_ptr_);
			}

		public:
			value_type* get() noexcept
			{
				return header_ptr_;
			}

			const value_type* get() const noexcept
			{
				return header_ptr_;
			}

			bool parse_bytes(flex_buffer_t& stream)
			{
				return header_ptr_->parse_bytes(stream);
			}

			bool to_bytes(flex_buffer_t& stream)
			{
				return header_ptr_->to_bytes(stream);
			}

		private:
			value_type* header_ptr_;
		};
	} // namespace impl
} // namespace aquarius