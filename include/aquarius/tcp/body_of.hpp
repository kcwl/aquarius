#pragma once
#include <aquarius/core/defines.hpp>
#include <aquarius/tcp/field.hpp>

namespace aquarius
{
	namespace tcp
	{
		struct null_body
		{};

		template <typename _Ty>
		class body_of : public fields<_Ty>
		{
			using value_type = _Ty;

		public:
			body_of() { this->alloc(body_ptr_); }

			body_of(const body_of&) = default;

			body_of(body_of&&) = default;

			~body_of() { this->dealloc(body_ptr_); }

		public:
			core::read_handle_result parse_bytes(core::flex_buffer_t& stream)
			{
				if constexpr (!std::same_as<value_type, null_body>)
				{
					if (!body_ptr_->ParseFromArray(stream.rdata(), static_cast<int>(stream.size())))
					{
						return core::read_handle_result::waiting_for_query;
					}

					stream.commit(static_cast<int>(body_ptr_->ByteSizeLong()));
				}

				return core::read_handle_result::ok;
			}

			core::read_handle_result to_bytes(core::flex_buffer_t& stream)
			{
				if constexpr (!std::same_as<value_type, null_body>)
				{
					auto buf = body_ptr_->SerializeAsString();

					elastic::binary_oarchive oa(stream);

					oa.append(buf.data());
				}

				return core::read_handle_result::ok;
			}

			value_type* get() noexcept { return body_ptr_; }

			const value_type* get() const noexcept { return body_ptr_; }

		private:
			value_type* body_ptr_;
		};
	} // namespace impl
} // namespace aquarius