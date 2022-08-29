#pragma once
#include "parse.hpp"

namespace aquarius
{
	namespace message
	{
		template<typename _Fields>
		class body_of
			: public _Fields
			, public ftstream_parse<_Fields>
		{
			using value_type = typename _Fields::value_type;

		public:
			body_of()
			{
				this->alloc(body_impl_);
			}

			body_of(const body_of&) = default;

			body_of(body_of&&) = default;

			~body_of()
			{
				this->dealloc(body_impl_);
			}

		public:
			template<typename _Stream>
			bool parse_bytes(_Stream& stream)
			{
				return ftstream_parse<_Fields>::parse_bytes(body_impl_, stream);
			}

			template<typename _Stream>
			bool to_bytes(_Stream& stream)
			{
				return ftstream_parse<_Fields>::to_bytes(body_impl_, stream);
			}

			value_type& get() noexcept
			{
				return *body_impl_;
			}

			const value_type& get() const noexcept
			{
				return *body_impl_;
			}

		private:
			value_type* body_impl_;
		};
	}
}