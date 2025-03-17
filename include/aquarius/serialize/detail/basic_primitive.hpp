#pragma once
#include <aquarius/serialize/detail/flex_buffer.hpp>

namespace aquarius
{
	namespace detail
	{
		template <typename _Elem, typename _Traits>
		class basic_primitive
		{
		public:
			using value_type = _Elem;

			using traits_t = _Traits;

		protected:
			class primitive_guard
			{
			public:
				primitive_guard(basic_primitive<value_type, traits_t>* primitive)
					: primitive_ptr_(primitive)
				{
					primitive_ptr_->start();
				}

				~primitive_guard()
				{
					primitive_ptr_->close();
				}

			private:
				basic_primitive<value_type, traits_t>* primitive_ptr_;
			};

		public:
			explicit basic_primitive(flex_buffer<value_type, traits_t>& bs)
				: streambuf_(bs)
			{}

			virtual ~basic_primitive() = default;

		public:
			void start()
			{
				streambuf_.start();
			}

			void close()
			{
				streambuf_.close();
			}

			void failed()
			{
				streambuf_.failed();
			}

		protected:
			flex_buffer<_Elem, _Traits>& streambuf_;
		};
	} // namespace detail
} // namespace aquarius