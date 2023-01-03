#pragma once
#include "parse.hpp"
#include <aquarius/proto/field.hpp>

namespace aquarius
{
	namespace proto
	{
		template<typename _Ty, typename _Parse>
		class body_of
			: public fields<_Ty>
		{
			using value_type = _Ty;

			using stream_type = typename _Parse::stream_type;

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
			virtual bool parse_bytes(stream_type& stream)
			{
				if (!body_ptr_->ParseFromArray(stream.data(),stream.size()))
				{
					return false;
				}

				stream.commit(body_ptr_->ByteSize());

				return true;
			}

			virtual bool to_bytes(stream_type& stream)
			{
				auto buf = body_ptr_->SerializeAsString();

				if (buf.empty())
					return false;

				elastic::binary_oarchive oa(stream);

				oa << buf;

				return true;
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
	}
}