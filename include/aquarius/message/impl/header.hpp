#pragma once
#include <aquarius/core/elastic.hpp>

namespace aquarius
{
	namespace impl
	{
		class basic_header
		{
		public:
			using virtual_base_type = basic_header;

		public:
			basic_header()
				: length_()
			{}

			virtual ~basic_header() = default;

		public:
			basic_header(basic_header&& other) noexcept
				: length_(other.length_)
			{
				other.length_ = 0;
			}

		public:
			void add_length(std::size_t length)
			{
				length_ += static_cast<uint16_t>(length);
			}

			uint16_t length() const
			{
				return length_;
			}

			void swap(basic_header& other)
			{
				std::swap(this->length_, other.length_);
			}

		protected:
			bool from_binary(flex_buffer_t& stream)
			{
				constexpr auto size_uint16 = sizeof(uint16_t);

				if (stream.size() < size_uint16)
					return false;

				std::memcpy((char*)&length_, stream.wdata(), size_uint16);

				stream.consume(size_uint16);

				if (length_ > stream.size())
				{
					stream.consume(-(static_cast<int>(size_uint16)));

					return false;
				}

				return true;
			}

			bool to_binary(flex_buffer_t& stream)
			{
				return elastic::to_binary(length_, stream);
			}

		protected:
			uint16_t length_;
		};
	} // namespace impl
} // namespace aquarius