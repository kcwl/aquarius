#pragma once
#include <aquarius/impl/field.hpp>
#include <io.h>

namespace aquarius
{
	namespace impl
	{
		template <typename _Ty, typename _Streambuf>
		class header_of : public fields<_Ty>
		{
		public:
			using streambuf_t = _Streambuf;

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

			virtual bool parse_bytes(streambuf_t& stream)
			{
				elastic::binary_iarchive ia(stream);
				ia >> *header_ptr_;

				return true;
			}

			virtual bool to_bytes(streambuf_t& stream)
			{
				elastic::binary_oarchive oa(stream);
				oa << *header_ptr_;

				return true;
			}

		private:
			value_type* header_ptr_;
		};
	} // namespace impl
} // namespace aquarius