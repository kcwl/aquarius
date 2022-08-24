#pragma once
#include "parse.hpp"

namespace aquarius
{
	namespace message
	{
		template<bool Request, typename _Fields>
		class header;

		template<typename _Fields>
		class header<true, _Fields>
			: public _Fields
			, public ftstream_parse<_Fields>
		{
		public:
			using message_type = std::true_type;

			using stream_type = typename ftstream_parse<_Fields>::stream_type;

			using value_type = typename _Fields::value_type;

		public:
			header()
			{
				this->alloc(header_ptr_);
			}

			header(const header&) = default;

			header(header&&) = default;

			header& operator=(const header&) = default;

			~header()
			{
				this->dealloc(header_ptr_);
			}

		public:
			value_type& get() noexcept
			{
				return *header_ptr_;
			}

			const value_type& get() const noexcept
			{
				return *header_ptr_;
			}

		protected:
			bool parse_bytes(stream_type& stream)
			{
				return ftstream_parse<_Fields>::parse_bytes(header_ptr_, stream);
			}

			bool to_bytes(stream_type& stream)
			{
				return ftstream_parse<_Fields>::to_bytes(header_ptr_, stream);
			}

		private:
			value_type* header_ptr_;
		};


		template<typename _Fields>
		class header<false, _Fields>
			: public _Fields
			, public ftstream_parse<_Fields>
		{
		public:
			using message_type = std::false_type;

			using stream_type = typename ftstream_parse<_Fields>::stream_type;

			using value_type = typename _Fields::value_type;

		public:
			header()
			{
				this->alloc(header_ptr_);
			}

			header(const header&) = default;

			header(header&&) = default;

			header& operator=(const header&) = default;

			~header()
			{
				this->dealloc(header_ptr_);
			}

		public:
			value_type& get() noexcept
			{
				return *header_ptr_;
			}

			const value_type& get() const noexcept
			{
				return *header_ptr_;
			}

		protected:
			bool parse_bytes(stream_type& stream)
			{
				return ftstream_parse<_Fields>::parse_bytes(header_ptr_, stream);
			}

			bool to_bytes(stream_type& stream)
			{
				return ftstream_parse<_Fields>::to_bytes(header_ptr_, stream);
			}

		private:
			value_type* header_ptr_;
		};
	}
}