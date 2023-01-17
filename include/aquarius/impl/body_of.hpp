#pragma once
#include <aquarius/impl/field.hpp>

namespace aquarius
{
	namespace impl
	{
		template <typename _Ty>
		class body_of : public fields<_Ty>
		{
			using value_type = _Ty;

		public:
			body_of()
			{
				this->alloc(body_ptr_);
			}

			body_of(const body_of&) = default;

			body_of(body_of&&) = default;

			~body_of()
			{
				this->dealloc(body_ptr_);
			}

		public:
			read_handle_result parse_bytes(flex_buffer_t& stream)
			{
				if constexpr (!std::same_as<value_type, void>)
				{
					if (!body_ptr_->ParseFromArray(stream.rdata(), static_cast<int>(stream.size())))
					{
						return read_handle_result::waiting_for_query;
					}

					stream.commit(static_cast<int>(body_ptr_->ByteSizeLong()));
				}
				
				return read_handle_result::ok;
			}

			read_handle_result to_bytes(flex_buffer_t& stream)
			{
				if constexpr (!std::same_as<value_type, void>)
				{
					auto buf = body_ptr_->SerializeAsString();

					elastic::binary_oarchive oa(stream);

					oa.append(buf.data());
				}
				
				return read_handle_result::ok;
			}

			value_type* get() noexcept
			{
				return body_ptr_;
			}

			const value_type* get() const noexcept
			{
				return body_ptr_;
			}

		private:
			value_type* body_ptr_;
		};
	} // namespace impl
} // namespace aquarius